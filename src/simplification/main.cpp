#include "../transfer_function/transfer_function.hpp"
#include <ginac/ginac.h>
#include <iostream>
#include <map>

#include "../symbol.hpp"
#include "simplification.hpp"
#include "weightmap.hpp"

int main()
{
    GiNaC::symbol R1 = get_symbol("R1");
    GiNaC::symbol R2 = get_symbol("R2");
    GiNaC::symbol C1 = get_symbol("C1");
    GiNaC::symbol C2 = get_symbol("C2");
    GiNaC::symbol gm = get_symbol("gm");
    GiNaC::symbol rout = get_symbol("rout");
    GiNaC::symbol gm1 = get_symbol("gm1");
    GiNaC::symbol gm2 = get_symbol("gm2");
    GiNaC::symbol rout1 = get_symbol("rout1");
    GiNaC::symbol rout2 = get_symbol("rout2");
    GiNaC::symbol Cl = get_symbol("Cl");
    GiNaC::symbol Cs = get_symbol("Cs");
    GiNaC::symbol sym1 = get_symbol("1");

    GiNaC::symbol s = get_symbol("s");

    std::vector<inequality> inequalities {
        { R1, R2, ">>" },
        { C1, C2, ">>" },
        { gm, sym1, ">>" },
        { rout, sym1, ">>" },
        { gm1, sym1, ">>" },
        { rout1, sym1, ">>" },
        { gm2, sym1, ">>" },
        { rout2, sym1, ">>" }
    };

    auto weightmap = compute_weightmap(inequalities);

    GiNaC::ex expr = (gm1 * rout1 * gm2 * rout2 + gm1 * rout1) / (1 + s * rout1 * Cl + s * rout2 * Cl + s * gm2 * rout2 * rout1 * Cl + s*s * rout1 * rout2 * Cs * Cl);
    transfer_function tf(expr);
    transfer_function simple_tf = simplify(tf, weightmap);

    std::cout << "before simplification:\n";
    tf.pretty_print(std::cout);
    std::cout << "after simplification:\n";
    simple_tf.pretty_print(std::cout);
}
