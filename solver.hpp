#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <string>

#include <ginac/ginac.h>

#include "netlist.hpp"

class solver
{
    public:
        solver(const std::string& mode, netlist&);
        void mna();
        void solve();
        void print();
        void print_matrices();

    private:
        std::string mode;
        netlist& nlist;
        GiNaC::matrix A;
        GiNaC::matrix x;
        GiNaC::matrix z;
        GiNaC::matrix results;
};

#endif // SOLVER_HPP
