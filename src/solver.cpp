#include "solver.hpp"

#include <boost/format.hpp>

#include "mna.hpp"
#include "symbol.hpp"
#include "util.hpp"

std::istream& operator>>(std::istream& stream, solver_mode& mode)
{
    std::string token;
    stream >> token;
    if(token == "ac")
    {
        mode = solve_ac;
    }
    else if(token == "zport")
    {
        mode = solve_zport;
    }
    else if(token == "yport")
    {
        mode = solve_yport;
    }
    else if(token == "sport")
    {
        mode = solve_sport;
    }
    else
    {
        std::cerr << "unknown simulation mode " << '"' << token << "\"\n";
        stream.setstate(std::ios_base::failbit);
    }
    return stream;
}

solver::solver(componentlist& components) :
    components(components)
{
}

component_types get_active_port(solver_mode mode)
{
    switch(mode)
    {
        case solve_zport:
            return ct_voltage_source;
            break;
        case solve_yport:
            return ct_current_source;
            break;
        case solve_sport: // TODO
            return ct_voltage_source;
            break;
        default:
            return ct_resistor; // dummy value, not needed
            break;
    }
}

component_types get_inactive_port(solver_mode mode)
{
    switch(mode)
    {
        case solve_zport:
            return ct_current_source;
            break;
        case solve_yport:
            return ct_voltage_source;
            break;
        case solve_sport: // TODO
            return ct_voltage_source;
            break;
        default:
            return ct_resistor; // dummy value, not needed
            break;
    }
}

GiNaC::matrix fun_solve_zport(const componentlist& components, nodemap& nmap)
{
    std::vector<component> ports = components.get_components_by_type(ct_port);
    component_types active_port = ct_voltage_source;
    component_types inactive_port = ct_current_source;

    GiNaC::matrix port_matrix(ports.size(), ports.size());

    for(unsigned int i = 0; i < ports.size(); ++i)
    {
        componentlist components_tmp(components);
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
        component p = ports[i];
        p.set_type(active_port);
        p.set_value(get_symbol("PORT"));
        components_tmp.add_component(p);
        // create matrices and solve
        GiNaC::matrix A = mna::create_A_matrix(nmap, components_tmp);
        GiNaC::matrix x = mna::create_x_vector(components_tmp);
        GiNaC::matrix z = mna::create_z_vector(nmap, components_tmp);

        GiNaC::matrix res = A.solve(x, z, GiNaC::solve_algo::gauss);
        for(unsigned int j = 0; j < ports.size(); ++j)
        {
            // get voltage
            GiNaC::ex numerator;
            if(j == i)
            {
                numerator = get_symbol("PORT");
            }
            else
            {
                component pp = ports[j];
                unsigned int node1 = nmap[pp.get_nodes()[0]];
                unsigned int node2 = nmap[pp.get_nodes()[1]];
                if(node1 != 0)
                {
                    numerator = res(node1 - 1, 0);
                }
                if(node2 != 0)
                {
                    numerator = numerator - res(node2 - 1, 0);
                }
            }

            // get current
            unsigned int iindex = components_tmp.network_size();
            GiNaC::ex denominator = -res(iindex - 1, 0);

            port_matrix(j, i) = numerator / denominator;
        }
    }
    return port_matrix;
}

GiNaC::matrix fun_solve_yport(const componentlist& components, nodemap& nmap)
{
    std::vector<component> ports = components.get_components_by_type(ct_port);
    component_types active_port = ct_current_source;
    component_types inactive_port = ct_voltage_source;

    GiNaC::matrix port_matrix(ports.size(), ports.size());

    for(unsigned int i = 0; i < ports.size(); ++i)
    {
        componentlist components_tmp(components);
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
        component p = ports[i];
        p.set_type(active_port);
        p.set_value(get_symbol("PORT"));
        components_tmp.add_component(p);
        // create matrices and solve
        GiNaC::matrix A = mna::create_A_matrix(nmap, components_tmp);
        GiNaC::matrix x = mna::create_x_vector(components_tmp);
        GiNaC::matrix z = mna::create_z_vector(nmap, components_tmp);

        GiNaC::matrix res = A.solve(x, z, GiNaC::solve_algo::gauss);
        for(unsigned int j = 0; j < ports.size(); ++j)
        {
            // get current
            GiNaC::ex numerator;
            if(j == i)
            {
                numerator = get_symbol("PORT");
            }
            else
            {
                numerator = -res(components_tmp.network_size() - 1, 0);
            }

            // get voltage
            unsigned int node1 = nmap[p.get_nodes()[0]];
            unsigned int node2 = nmap[p.get_nodes()[1]];
            GiNaC::ex denominator;
            if(node1 != 0)
            {
                denominator = res(node1 - 1, 0);
            }
            if(node2 != 0)
            {
                denominator = denominator - res(node2 - 1, 0);
            }
            port_matrix(j, i) = numerator / denominator;
        }
    }
    return port_matrix;
}

result solver::solve(bool print)
{
    GiNaC::matrix zmatrix = fun_solve_zport(components, nmap);
    GiNaC::matrix ymatrix = fun_solve_yport(components, nmap);

    // always do a regular AC simulation
    GiNaC::matrix A = mna::create_A_matrix(nmap, components);
    GiNaC::matrix x = mna::create_x_vector(components);
    GiNaC::matrix z = mna::create_z_vector(nmap, components);
    if(print)
    {
        print_network_matrices(A, x, z);
    }
    GiNaC::matrix res = A.solve(x, z, GiNaC::solve_algo::gauss);

    result results(components, res, nmap);
    boost::format fmter = boost::format("%d,%d");
    unsigned int number_of_ports = 2; // FIXME
    for(unsigned int i = 0; i < number_of_ports; ++i)
    {
        for(unsigned int j = 0; j < number_of_ports; ++j)
        {
            std::string key = str(fmter % (i + 1) % (j + 1));
            results.add(key, zmatrix(i, j));
        }
    }

    return results;
}
