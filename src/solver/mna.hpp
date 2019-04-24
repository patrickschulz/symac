#ifndef MNA_HPP
#define MNA_HPP

#include <vector>
#include <ginac/ginac.h>

#include "../component.hpp"

class componentlist;
class nodemap;

unsigned int get_element_size(const component& c);

namespace mna {
    GiNaC::matrix create_A_matrix(nodemap& nmap, const componentlist& components);
    GiNaC::matrix create_x_vector(nodemap& nmap, const componentlist& components);
    GiNaC::matrix create_z_vector(nodemap& nmap, const componentlist& components);
}

#endif // MNA_HPP
