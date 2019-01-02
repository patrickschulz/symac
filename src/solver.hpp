#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <string>
#include <vector>
#include <iostream>

#include <ginac/ginac.h>

#include "componentlist.hpp"
#include "nodemap.hpp"
#include "result.hpp"

class solver
{
    public:
        solver(componentlist& components);
        result solve(bool print);
        void print_matrices();

    private:
        componentlist& components;
        nodemap nmap;
};

#endif // SOLVER_HPP
