#ifndef NOISE_HPP
#define NOISE_HPP

#include <ginac/ginac.h>

#include "common.hpp"

#include "../result.hpp"
#include "../transfer_function/transfer_function.hpp"

GiNaC::ex integrate_NTF_sabs(const transfer_function& tf);
GiNaC::ex integrate_NTF_sabs(const GiNaC::ex& e);
GiNaC::ex squared_abs(const GiNaC::ex& e);

void solve_noise(const componentlist& components, nodemap& nmap, result& results);

#endif // NOISE_HPP
