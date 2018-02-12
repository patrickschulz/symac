#ifndef MNA_HPP
#define MNA_HPP

#include <ginac/ginac.h>

GiNaC::matrix create_A_matrix(const netlist& nlist);
GiNaC::matrix create_x_vector(const netlist& nlist);
GiNaC::matrix create_z_vector(const netlist& nlist);

#endif // MNA_HPP
