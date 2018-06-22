#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <string>
#include <vector>

#include <ginac/ginac.h>

#include "netlist.hpp"

class solver
{
    public:
        solver(const std::string& mode, const netlist&,const std::vector<std::string> nodes);
        void mna();
        void solve();
        void print();
        void print_matrices();
        void to_matlab(const std::string& filename);

    private:
        std::string mode;
        const netlist& nlist;
        std::vector<std::string> nodes;
        GiNaC::matrix A;
        GiNaC::matrix x;
        GiNaC::matrix z;
        GiNaC::matrix results;
};

#endif // SOLVER_HPP
