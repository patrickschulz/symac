#include "common.hpp"

#include "mna.hpp"
#include "util.hpp"

GiNaC::matrix solve_network(componentlist components, nodemap& nmap, bool linearize, bool print)
{
    // linearize circuit
    if(linearize)
    {
        componentlist newcomponents;
        for(const component& c : components)
        {
            newcomponents.add_component(get_small_signal_model(c));
        }
        components = newcomponents;
    }

    GiNaC::matrix A = mna::create_A_matrix(nmap, components);
    GiNaC::matrix x = mna::create_x_vector(nmap, components);
    GiNaC::matrix z = mna::create_z_vector(nmap, components);

    if(print)
    {
        print_network_matrices(A, x, z);
    }

    GiNaC::matrix res(A.rows(), 1);
    try
    {
        res = A.solve(x, z, GiNaC::solve_algo::gauss);
    }
    catch(std::runtime_error)
    {
        std::cerr << "could not solve network, inconsistent linear system:\n";
        print_network_matrices(A, x, z);
    }
    return res;
}

