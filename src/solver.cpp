#include "solver.hpp"

#include <boost/format.hpp>

#include "mna.hpp"
#include "symbol.hpp"
#include "util.hpp"
#include "transfer_function/transfer_function.hpp"
#include "noise.hpp"

const std::string portdummynode("DUMMYPORTNODE");

solver::solver(const componentlist& components) :
    components(components)
{
}

GiNaC::matrix solve_network(const componentlist& components, nodemap& nmap, bool print)
{
    GiNaC::matrix A = mna::create_A_matrix(nmap, components);
    GiNaC::matrix x = mna::create_x_vector(components);
    GiNaC::matrix z = mna::create_z_vector(nmap, components);

    if(print)
    {
        print_network_matrices(A, x, z);
    }

    GiNaC::matrix res(A.rows(), 1);
    try
    {
        res = A.solve(x, z, GiNaC::solve_algo::gauss);
    }
    catch(std::runtime_error)
    {
        std::cerr << "could not solve network, inconsistent linear system:\n";
        print_network_matrices(A, x, z);
    }
    return res;
}

enum port_mode
{
    zport,
    yport,
    sport
};

GiNaC::ex get_port_voltage(const component& pp, const GiNaC::matrix& res, nodemap& nmap)
{
    unsigned int node1 = nmap[pp.get_nodes()[0]];
    unsigned int node2 = nmap[pp.get_nodes()[1]];
    GiNaC::ex value = 0;
    if(node1 != 0)
    {
        value = res(node1 - 1, 0);
    }
    if(node2 != 0)
    {
        value = value - res(node2 - 1, 0);
    }
    return value;
}

GiNaC::ex get_nport_numerator(port_mode mode, unsigned int i, unsigned int j, const componentlist& components, const GiNaC::matrix& res, nodemap& nmap)
{
    std::vector<component> ports = components.get_components_by_type(ct_port);
    switch(mode)
    {
        case zport:
            if(j == i)
            {
                return ports[i].get_value();
            }
            else
            {
                return get_port_voltage(ports[j], res, nmap);
            }
        case yport:
            if(j == i)
            {
                return ports[i].get_value();
            }
            else
            {
                return -res(components.network_size() - 1, 0);
            }
        case sport:
        {
            component p = ports[j];
            unsigned int inodep = nmap[p.get_nodes()[0]];

            GiNaC::ex vj, ij;
            GiNaC::possymbol Z0 = get_symbol("Z0");

            if(j == i)
            {
                unsigned int inoden = nmap[portdummynode];

                // get voltage
                vj = res(inodep - 1, 0);

                // get current
                ij = (res(inoden - 1, 0) - res(inodep - 1, 0)) / Z0; // notice direction of current
            }
            else
            {
                vj = res(inodep - 1, 0);
                ij = -res(inodep - 1, 0) / Z0; // notice direction of current
            }

            return vj - ij * Z0;
        }
        default: // dummy
            return GiNaC::ex();
    }
}

GiNaC::ex get_nport_denominator(port_mode mode, unsigned int i, unsigned int j, const componentlist& components, const GiNaC::matrix& res, nodemap& nmap)
{
    std::vector<component> ports = components.get_components_by_type(ct_port);
    (void) j;
    switch(mode)
    {
        case zport:
            return -res(components.network_size() - 1, 0);
        case yport:
            return get_port_voltage(ports[i], res, nmap);
        case sport:
        {
            component p = ports[i];
            unsigned int inodep = nmap[p.get_nodes()[0]];
            unsigned int inoden = nmap[portdummynode];
            GiNaC::possymbol Z0 = get_symbol("Z0");

            // get voltage
            GiNaC::ex vi = res(inodep - 1, 0);

            // get current
            GiNaC::ex ii = (res(inoden - 1, 0) - res(inodep - 1, 0)) / Z0; // notice direction of current

            return vi + ii * Z0;
        }
        default: // dummy
            return GiNaC::ex();
    }
}

void insert_active_port(componentlist& components_tmp, port_mode mode, unsigned int i, const std::vector<component>& ports)
{
    component p = ports[i];
    std::vector<component> for_insertion;
    switch(mode)
    {
        case zport:
            p.set_type(ct_voltage_source);
            for_insertion.push_back(p);
            break;
        case yport:
            p.set_type(ct_current_source);
            for_insertion.push_back(p);
            break;
        case sport:
        {
            component pres = p;

            auto nodes = p.get_nodes();
            std::string nodep = nodes[0];
            std::string noden = nodes[1];
            std::string dummynode(portdummynode);

            nodes[0] = dummynode;
            nodes[1] = noden;
            p.set_nodes(nodes);
            p.set_type(ct_voltage_source);
            for_insertion.push_back(p);

            nodes[0] = nodep;
            nodes[1] = dummynode;
            pres.set_nodes(nodes);
            pres.set_type(ct_resistor);
            pres.set_value(get_symbol("Z0"));
            for_insertion.push_back(pres);
            break;
        }
        default: // dummy
            break;
    }
    for(auto& ci : for_insertion)
    {
        components_tmp.add_component(ci);
    }
}

void insert_inactive_port(componentlist& components_tmp, port_mode mode, unsigned int j, const std::vector<component>& ports)
{
    component p = ports[j];
    p.set_value(0);
    switch(mode)
    {
        case zport:
            p.set_type(ct_current_source);
            break;
        case yport:
            p.set_type(ct_voltage_source);
            break;
        case sport:
            p.set_type(ct_resistor);
            p.set_value(get_symbol("Z0"));
        default: // dummy
            break;
    }
    components_tmp.add_component(p);
}

GiNaC::matrix solve_nport(port_mode mode, const componentlist& components, nodemap& nmap, bool print)
{
    std::vector<component> ports = components.get_components_by_type(ct_port);

    GiNaC::matrix port_matrix(ports.size(), ports.size());

    for(unsigned int i = 0; i < ports.size(); ++i)
    {
        componentlist components_tmp(components);
        insert_active_port(components_tmp, mode, i, ports);
        for(unsigned int j = 0; j < ports.size(); ++j)
        {
            if(j != i)
            {
                insert_inactive_port(components_tmp, mode, j, ports);
            }
        }

        /* solve network and calculate parameters */
        GiNaC::matrix res = solve_network(components_tmp, nmap, print);
        for(unsigned int j = 0; j < ports.size(); ++j)
        {
            GiNaC::ex num = get_nport_numerator(mode, i, j, components_tmp, res, nmap);
            GiNaC::ex den = get_nport_denominator(mode, i, j, components_tmp, res, nmap);
            if(!den.is_zero())
            {
                port_matrix(j, i) = num / den;
            }
        }
    }
    return port_matrix;
}

result solver::solve(bool print)
{
    // regular AC simulation
    GiNaC::matrix res = solve_network(components, nmap, print);
    result results(components, res, nmap);

    // n-port simulations
    std::vector<std::pair<port_mode, std::string>> matrix_container {
        { zport, "Z" },
        { yport, "Y" },
        { sport, "S" }
    };
    for(auto& M : matrix_container)
    {
        GiNaC::matrix nmatrix = solve_nport(M.first, components, nmap, print);

        boost::format fmter = boost::format("%d,%d");
        unsigned int number_of_ports = components.number_of_components(ct_port);
        for(unsigned int i = 0; i < number_of_ports; ++i)
        {
            for(unsigned int j = 0; j < number_of_ports; ++j)
            {
                std::string key = str(fmter % (i + 1) % (j + 1));
                results.add(M.second, key, nmatrix(i, j));
            }
        }
    }

    // noise simulation
    GiNaC::ex totalintegratednoise = 0;
    for(const component& c : components)
    {
        if(c.is_noisy())
        {
            componentlist components_tmp(components);

            GiNaC::ex noise = c.get_noise();
            component source = c;
            source.set_type(ct_current_source);
            source.set_value(noise);
            components_tmp.add_component(source);

            /* solve network and calculate parameters */
            GiNaC::matrix res = solve_network(components_tmp, nmap, print);

            for(unsigned int node = 0; node < components_tmp.number_of_nodes(); ++node)
            {
                GiNaC::ex value = res(node, 0);
                transfer_function NTF = value / noise;
                //std::cout << NTF << '\n';
            }

            /*
            unsigned int node = nmap["vx"];
            GiNaC::ex value = res(node - 1, 0);
            transfer_function NTF = value / noise;
            totalintegratednoise += integrate_NTF_sabs(NTF) * noise;
            */
        }
    }
    //std::cout << collect_common_factors(totalintegratednoise) << '\n';

    return results;
}
