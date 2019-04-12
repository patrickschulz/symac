#ifndef UTIL_HPP
#define UTIL_HPP

#include <ginac/ginac.h>

#include "../nodemap.hpp"

void print_network_matrices(const GiNaC::matrix& A, const GiNaC::matrix& x, const GiNaC::matrix& z);

#endif // UTIL_HPP
