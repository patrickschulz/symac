#include "solver.hpp"

#include "common.hpp"
#include "nport.hpp"

#include <boost/format.hpp>

#include "mna.hpp"
#include "../symbol.hpp"
#include "util.hpp"
#include "../transfer_function/transfer_function.hpp"
#include "noise.hpp"

solver::solver(componentlist components) :
    components(components)
{
}

result solver::solve(bool linearize, bool print)
{
    // linearize circuit
    if(linearize)
    {
        componentlist newcomponents;
        for(const component& c : components)
        {
            newcomponents.add_component(get_small_signal_model(c));
        }
        components = newcomponents;
    }

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

    solve_noise(components, nmap, results);
    // noise simulation
    /*
    for(unsigned int node = 0; node < components.number_of_nodes(); ++node)
    {
        GiNaC::ex totalnoise = 0;
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

                // solve network and calculate parameters
                GiNaC::matrix res = solve_network(components_tmp, nmap, print);

                GiNaC::ex value = res(node, 0);
                GiNaC::ex NTF = value / noise;
                totalnoise += squared_abs(NTF) * noise;
                totalintegratednoise += integrate_NTF_sabs(NTF) * noise;

                // add NTF
                boost::format fmter = boost::format("%s,%s");
                std::string key = str(fmter % c.get_name() % nmap[node + 1]);
                results.add("NTF", key, NTF);
            }
        }
        results.add("VN", nmap[node + 1], totalnoise);
        results.add("VNI", nmap[node + 1], totalintegratednoise);
    }
    */

    return results;
}
