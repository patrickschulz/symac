#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <string>
#include <vector>
#include <iostream>

#include <ginac/ginac.h>

#include "../componentlist.hpp"
#include "../nodemap.hpp"
#include "../result.hpp"

class solver
{
    public:
        solver(const componentlist& components, result& results);
        void solve(bool linearize, bool print);

    private:
        const componentlist& components;
        result& results;
        nodemap nmap;
};

#endif // SOLVER_HPP
