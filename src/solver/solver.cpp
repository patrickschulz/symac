#include "solver.hpp"

#include "common.hpp"
#include "ac.hpp"
#include "nport.hpp"
#include "noise.hpp"

solver::solver(const componentlist& components, result& results) :
    components(components), results(results)
{
}

void solver::solve(bool linearize, bool print)
{
    solve_ac   (components, nmap, results, linearize, print);
    solve_nport(components, nmap, results, linearize, print);
    solve_noise(components, nmap, results, linearize, print);
}
