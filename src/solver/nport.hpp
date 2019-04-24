#ifndef SOLVE_NPORT_HPP
#define SOLVE_NPORT_HPP

#include <ginac/ginac.h>

#include "../componentlist.hpp"
#include "../nodemap.hpp"
#include "../result.hpp"

enum port_mode
{
    zport,
    yport,
    sport,
    tport,
    abcdport,
    hport,
    gport
};

void solve_nport(const componentlist& components, nodemap& nmap, result& results, bool linearize, bool print);

#endif // SOLVE_NPORT_HPP
