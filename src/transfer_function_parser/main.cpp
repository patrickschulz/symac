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
    GiNaC::symbol R1("R1");
    GiNaC::symbol R2("R2");
    GiNaC::symbol C1("C1");
    GiNaC::symbol C2("C2");
    GiNaC::symbol A0("A0");
    GiNaC::ex expr = A0 * (1 + s * R1 * C1 + s * R2 * C2) / (A0 * (1 + s * R1 * C1 + s*s * R1 * R2 * C1 * C2));

    transfer_function tf(expr);

    tf.pretty_print(std::cout);
}

