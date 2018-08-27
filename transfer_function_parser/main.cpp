#include "product.hpp"
#include "polynom.hpp"
#include "transfer_function.hpp"

#include "sspace_symbols.hpp"

GiNaC::symbol s = GiNaC::symbol("s");

int main()
{
    GiNaC::symbol R = GiNaC::symbol("R");
    GiNaC::symbol C = GiNaC::symbol("C");

    GiNaC::ex tfex = (1 + s * R * C)/(1 + s * R * C + s * s * R * R * C * C);

    transfer_function tf(tfex);

    //std::cout << tf << '\n';
}
