#include <iostream>

#include "transfer_function.hpp"
#include "../symbol.hpp"

int main()
{
    GiNaC::symbol s = get_symbol("s");
    GiNaC::symbol gm1("gm1");
    GiNaC::symbol gm2("gm2");
    GiNaC::symbol rout1("rout1");
    GiNaC::symbol rout2("rout2");
    GiNaC::symbol Cl("Cl");
    GiNaC::symbol vin("vin");
    GiNaC::ex expr = (-gm1 * rout1 * vin - vin * s * gm1 * rout1 * rout2 * Cl) / (1 + s * (Cl * rout1 + Cl * rout2 + gm2 * rout2 * Cl * rout1));

    transfer_function tf(expr);

    tf.pretty_print(std::cout, "V(vs) = ");
}

