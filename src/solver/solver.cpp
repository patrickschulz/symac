#include "solver.hpp"

#include "common.hpp"
#include "nport.hpp"
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

    solve_nport(components, nmap, results);
    solve_noise(components, nmap, results);

    return results;
}
