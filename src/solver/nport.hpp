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
    sport
};

GiNaC::ex get_port_voltage(const component& pp, const GiNaC::matrix& res, nodemap& nmap);
GiNaC::ex get_nport_numerator(port_mode mode, unsigned int i, unsigned int j, const componentlist& components, const GiNaC::matrix& res, nodemap& nmap);
GiNaC::ex get_nport_denominator(port_mode mode, unsigned int i, unsigned int j, const componentlist& components, const GiNaC::matrix& res, nodemap& nmap);
void insert_active_port(componentlist& components_tmp, port_mode mode, unsigned int i, const std::vector<component>& ports);
void insert_inactive_port(componentlist& components_tmp, port_mode mode, unsigned int j, const std::vector<component>& ports);
GiNaC::matrix solve_nport_single(port_mode mode, const componentlist& components, nodemap& nmap, bool print);

void solve_nport(const componentlist& components, nodemap& nmap, result& results);

#endif // SOLVE_NPORT_HPP