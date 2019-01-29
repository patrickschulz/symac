#include <iostream>

#include "../transfer_function/transfer_function.hpp"
#include "../symbol.hpp"

int main()
{
    auto s = get_symbol("s");
    auto R = get_symbol("R");
    auto C = get_symbol("C");
    GiNaC::ex expr = 1 / (1 + s * R * C);
    transfer_function tf(expr);

    polynom num = tf.get_numerator();
}
