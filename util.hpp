#ifndef UTIL_HPP
#define UTIL_HPP

#include <vector>
#include <string>
#include <ginac/ginac.h>
#include <algorithm> 
#include <cctype>
#include <locale>

#include "netlist.hpp"
#include "result.hpp"

//void print_matrix(const GiNaC::matrix&);
//void print_matrix(const GiNaC::matrix&, const std::string&);
void print_results(const netlist&, const GiNaC::matrix&, const GiNaC::matrix&, const std::vector<result>&);
void print_results(const netlist&, const GiNaC::matrix&);
void create_report(const netlist&, const GiNaC::matrix&);

void ltrim(std::string &s); // trim from start (in place)
void rtrim(std::string &s); // trim from end (in place)
void trim(std::string &s);  // trim from both ends (in place)

#endif // UTIL_HPP
