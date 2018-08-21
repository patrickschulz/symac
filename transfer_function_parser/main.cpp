#include "product.hpp"
#include "polynom.hpp"
#include "transfer_function.hpp"

#include "sspace_symbols.hpp"

GiNaC::symbol s = GiNaC::symbol("s");

int main()
{
    GiNaC::symbol R1 = GiNaC::symbol("R1");
    GiNaC::symbol R2 = GiNaC::symbol("R2");
    GiNaC::symbol C1 = GiNaC::symbol("C1");
    GiNaC::symbol C2 = GiNaC::symbol("C2");

    GiNaC::ex tfex = (1 + s)/(1 + s * s);

    transfer_function tf(tfex);

    std::cout << tf << '\n';
}
