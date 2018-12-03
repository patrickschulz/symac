#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <string>
#include <vector>

#include <ginac/ginac.h>

#include "componentlist.hpp"
#include "nodemap.hpp"
#include "result.hpp"

enum solver_mode
{
    solve_ac,
    solve_noise,
    solve_nport
};

class solver
{
    public:
        solver(componentlist& components);
        result solve(solver_mode);
        void print_matrices();

    private:
        componentlist& components;
        nodemap nmap;

        GiNaC::matrix A;
        GiNaC::matrix x;
        GiNaC::matrix z;
};

#endif // SOLVER_HPP
