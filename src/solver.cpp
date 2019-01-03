#include "solver.hpp"

#include <boost/format.hpp>

#include "mna.hpp"
#include "symbol.hpp"
#include "util.hpp"

solver::solver(componentlist& components) :
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

    GiNaC::matrix res = A.solve(x, z, GiNaC::solve_algo::gauss);
    return res;
}

enum port_mode
{
    zport,
    yport
};

std::pair<component_types, component_types> get_port_sources(port_mode mode)
{
    switch(mode)
    {
        case zport:
            return { ct_voltage_source, ct_current_source };
        case yport:
            return { ct_current_source, ct_voltage_source };
        default: // dummy
            return {};
    }
}

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
    if(j == i)
    {
        return ports[i].get_value();
    }
    else
    {
        switch(mode)
        {
            case zport:
                return get_port_voltage(ports[j], res, nmap);
            case yport:
                return -res(components.network_size() - 1, 0);
            default: // dummy
                return GiNaC::ex();
        }
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
        default: // dummy
            return GiNaC::ex();
    }
}

GiNaC::matrix solve_nport(port_mode mode, const componentlist& components, nodemap& nmap)
{
    std::vector<component> ports = components.get_components_by_type(ct_port);
    component_types active_port, inactive_port;
    std::tie(active_port, inactive_port) = get_port_sources(mode);

    GiNaC::matrix port_matrix(ports.size(), ports.size());

    for(unsigned int i = 0; i < ports.size(); ++i)
    {
        componentlist components_tmp(components);
        // replace ports with sources
        // active ports
        component p = ports[i];
        p.set_type(active_port);
        components_tmp.add_component(p);
        // inactive ports
        for(unsigned int j = 0; j < ports.size(); ++j)
        {
            if(j != i)
            {
                component c = ports[j];
                c.set_type(inactive_port);
                c.set_value(0);
                components_tmp.add_component(c);
            }
        }

        GiNaC::matrix res = solve_network(components_tmp, nmap, false);
        for(unsigned int j = 0; j < ports.size(); ++j)
        {
            GiNaC::ex num = get_nport_numerator(mode, i, j, components_tmp, res, nmap);
            GiNaC::ex den = get_nport_denominator(mode, i, j, components_tmp, res, nmap);
            port_matrix(j, i) = num / den;
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
        { yport, "Y" }
    };
    for(auto& M : matrix_container)
    {
        GiNaC::matrix nmatrix = solve_nport(M.first, components, nmap);

        boost::format fmter = boost::format("%s%d,%d");
        unsigned int number_of_ports = components.number_of_components(ct_port);
        for(unsigned int i = 0; i < number_of_ports; ++i)
        {
            for(unsigned int j = 0; j < number_of_ports; ++j)
            {
                std::string key = str(fmter % M.second % (i + 1) % (j + 1));
                results.add(key, nmatrix(i, j));
            }
        }
    }

    return results;
}
