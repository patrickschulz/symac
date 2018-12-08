#include "solver.hpp"

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

result solver::solve(solver_mode mode, bool print)
{
    switch(mode)
    {
        case solve_zport:
        case solve_yport:
        case solve_sport:
        {
            std::vector<component> ports = components.get_components_by_type(ct_port);
            component_types active_port = get_active_port(mode);
            component_types inactive_port = get_inactive_port(mode);
            if(ports.size() > 0)
            {
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
                    if(print)
                    {
                        std::cout << i + 1 << ". iteration\n";
                        print_network_matrices(A, x, z);
                    }
                    GiNaC::matrix res = A.solve(x, z, GiNaC::solve_algo::gauss);
                    for(unsigned int j = 0; j < ports.size(); ++j)
                    {
                        GiNaC::ex parameter;
                        if(mode == solve_zport)
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

                            parameter = numerator / denominator;
                        }
                        if(mode == solve_yport)
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
                            parameter = numerator / denominator;
                        }
                        port_matrix(j, i) = parameter;
                    }
                }
                std::cout << port_matrix << '\n';
            }
            break;
        }
        case solve_ac:
        {
            // regular AC
            GiNaC::matrix A = mna::create_A_matrix(nmap, components);
            GiNaC::matrix x = mna::create_x_vector(components);
            GiNaC::matrix z = mna::create_z_vector(nmap, components);
            if(print)
            {
                print_network_matrices(A, x, z);
            }
            GiNaC::matrix res = A.solve(x, z, GiNaC::solve_algo::gauss);
            return result(components, res, nmap);
        }
        case solve_noise:
        {
            break;
        }
    }
    // FIXME: this is just a dummy to ensure correct compilation
    // This will be changed once a unified result system for all simulation types is established
    GiNaC::matrix A = mna::create_A_matrix(nmap, components);
    GiNaC::matrix x = mna::create_x_vector(components);
    GiNaC::matrix z = mna::create_z_vector(nmap, components);
    GiNaC::matrix res = A.solve(x, z, GiNaC::solve_algo::gauss);
    return result(components, res, nmap);
}
