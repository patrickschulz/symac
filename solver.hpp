#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <string>
#include <vector>

#include <ginac/ginac.h>

#include "netlist.hpp"

class solver
{
    public:
        solver(const std::string& mode, const netlist&);
        void mna();
        void solve();
        void print();
        void print_matrices();
        void mat_export();

    private:
        std::string mode;
        const netlist& nlist;
        GiNaC::matrix A;
        GiNaC::matrix x;
        GiNaC::matrix z;
        GiNaC::matrix results;
};

#endif // SOLVER_HPP
