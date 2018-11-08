#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <string>
#include <vector>

#include <ginac/ginac.h>

#include "componentlist.hpp"
#include "nodemap.hpp"
#include "result.hpp"

class solver
{
    public:
        solver(const componentlist& components);
        void mna();
        result solve();
        void print_matrices();

    private:
        const componentlist& components;
        nodemap nmap;

        GiNaC::matrix A;
        GiNaC::matrix x;
        GiNaC::matrix z;
};

#endif // SOLVER_HPP
