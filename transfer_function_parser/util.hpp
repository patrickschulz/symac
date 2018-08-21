#ifndef UTIL_HPP
#define UTIL_HPP

#include <ginac/ginac.h>
#include <iostream>
#include <vector>

#include "sspace_symbols.hpp"

void debug_print_internal(const GiNaC::ex& e)
{
    std::cout << GiNaC::ex_to<GiNaC::basic>(e).class_name();
    std::cout << "(";
    size_t n = e.nops();
    if (n)
    {
        for (size_t i=0; i<n; i++)
        {
            debug_print_internal(e.op(i));
            if (i != n-1)
            {
                std::cout << ",";
            }
        }
    }
    else
    {
        std::cout << e;
    }
    std::cout << ")";
}

void debug_print(const GiNaC::ex & e)
{
    debug_print_internal(e);
    std::cout << '\n';
}

std::string get_ginac_type(const GiNaC::ex& e)
{
    std::string type = GiNaC::ex_to<GiNaC::basic>(e).class_name();
    return type;
}

#endif // UTIL_HPP
