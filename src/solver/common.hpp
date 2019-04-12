#ifndef SOLVE_COMMON_HPP
#define SOLVE_COMMON_HPP

#include <ginac/ginac.h>

#include "../componentlist.hpp"
#include "../nodemap.hpp"

GiNaC::matrix solve_network(componentlist components, nodemap& nmap, bool linearize, bool print);

#endif // SOLVE_COMMON_HPP
