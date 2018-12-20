#include "../transfer_function/transfer_function.hpp"
#include <ginac/ginac.h>
#include <iostream>
#include <map>

#include "simplification.hpp"

template<typename T>
void print_vector(const std::vector<T>& vec)
{
    std::cout << "{ ";
    for(const auto& t : vec)
    {
        std::cout << t << ", ";
    }
    std::cout << "}\n";
}

int main()
{
    GiNaC::symbol R1("R1");
    GiNaC::symbol R2("R2");
    GiNaC::symbol C1("C1");
    GiNaC::symbol C2("C2");
    GiNaC::symbol gm("gm");
    GiNaC::symbol rout("rout");
    GiNaC::symbol gm1("gm1");
    GiNaC::symbol gm2("gm2");
    GiNaC::symbol rout1("rout1");
    GiNaC::symbol rout2("rout2");
    GiNaC::symbol Cl("Cl");
    GiNaC::symbol Cs("Cs");

    std::map<GiNaC::symbol, int, GiNaC::ex_is_less> weightmap
    {
        { R1, 1 },
        { R2, 0 },
        { C1, 1 },
        { C2, 0 },
        { gm, 1 },
        { rout, 1 },
        { gm1, 1 },
        { rout1, 1 },
        { gm2, 1 },
        { rout2, 1 },
        { Cl, 0 },
        { Cs, 0 }
    };

    std::vector<GiNaC::ex> expressions
    {
        { R1 * C1 + R2 * C2 },
        { R1 * C1 + gm * rout * R1 * C2 },
        { rout1 * Cl + rout1 * Cs + rout2 * Cl + gm2 * rout2 * rout1 * Cl }
    };

    for(const auto& expr : expressions)
    {
        sum s = parse_sum(expr);

        auto weights = calculate_weights(s, weightmap);
        auto indices = calculate_indices_to_keep(weights);
        sum ns = create_new_expression(s, indices);

        std::cout << s << " -----> " << ns << '\n';
    }
}
