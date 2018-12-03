#ifndef MNA_HPP
#define MNA_HPP

#include <vector>
#include <ginac/ginac.h>

#include "component.hpp"

class componentlist;
class nodemap;

namespace mna {
    GiNaC::matrix create_A_matrix(nodemap& nmap, const componentlist& components);
    GiNaC::matrix create_x_vector(const componentlist& components);
    GiNaC::matrix create_z_vector(nodemap& nmap, const componentlist& components);
}

#endif // MNA_HPP
