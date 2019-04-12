#include "solver.hpp"

#include "common.hpp"
#include "nport.hpp"
#include "noise.hpp"

solver::solver(const componentlist& components) :
    components(components)
{
}

result solver::solve(bool linearize, bool print)
{
    // regular AC simulation
    GiNaC::matrix res = solve_network(components, nmap, linearize, print);
    result results(components, res, nmap);

    solve_nport(components, nmap, results, linearize, print);
    solve_noise(components, nmap, results, linearize, print);

    return results;
}
