#ifndef UTIL_HPP
#define UTIL_HPP

#include <vector>
#include <ginac/ginac.h>

#include "netlist.hpp"
#include "result.hpp"

//void print_matrix(const GiNaC::matrix&);
//void print_matrix(const GiNaC::matrix&, const std::string&);
void print_results(const netlist&, const GiNaC::matrix&, const GiNaC::matrix&, const std::vector<result>&);
void print_results(const netlist&, const GiNaC::matrix&);
void create_report(const netlist&, const GiNaC::matrix&);

#endif // UTIL_HPP
