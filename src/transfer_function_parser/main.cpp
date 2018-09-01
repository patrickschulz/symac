#include <iostream>

#include "sum.hpp"
#include "product.hpp"
#include "polynom.hpp"
#include "transfer_function.hpp"

#include "sspace_symbols.hpp"

void ginacprint(const GiNaC::symbol& sym)
{
    std::cerr << sym << '\n';
}

void ginacprint(const GiNaC::numeric& num)
{
    std::cerr << num << '\n';
}

GiNaC::symbol s = GiNaC::symbol("s");

int main()
{
    GiNaC::symbol R1 = GiNaC::symbol("R1");
    GiNaC::symbol R2 = GiNaC::symbol("R2");
    GiNaC::symbol C1 = GiNaC::symbol("C1");
    GiNaC::symbol C2 = GiNaC::symbol("C2");

    GiNaC::ex den1 = s * R1;
    GiNaC::ex den2 = s * R1 * C1;
    GiNaC::ex den3 = s * (R1 + R2);
    GiNaC::ex den4 = s * (R1 * C1 + R2 * C2);
    GiNaC::ex den5 = s * ((R1 + R2) * C1);

    debug_print(den1.coeff(s, 1));
    debug_print(den2.coeff(s, 1));
    debug_print(den3.coeff(s, 1));
    debug_print(den4.coeff(s, 1));
    debug_print(den5.coeff(s, 1));
}
