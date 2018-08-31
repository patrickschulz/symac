#ifndef MNA_HPP
#define MNA_HPP

#include <ginac/ginac.h>

namespace mna {
    GiNaC::matrix create_A_matrix(unsigned int networksize, const std::vector<std::unique_ptr<component>>& components);
    GiNaC::matrix create_x_vector(unsigned int networksize, unsigned int number_of_nodes, const std::vector<std::unique_ptr<component>>& components);
    GiNaC::matrix create_z_vector(unsigned int networksize, unsigned int number_of_nodes, const std::vector<std::unique_ptr<component>>& components);
}

#endif // MNA_HPP
