#ifndef MNA_HPP
#define MNA_HPP

#include <ginac/ginac.h>

#include "componentlist.hpp"

namespace mna {
    GiNaC::matrix create_A_matrix(const componentlist& components);
    GiNaC::matrix create_x_vector(const componentlist& components);
    GiNaC::matrix create_z_vector(const componentlist& components);
}

#endif // MNA_HPP
