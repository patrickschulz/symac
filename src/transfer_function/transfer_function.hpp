#ifndef TRANSFER_FUNCTION_HPP
#define TRANSFER_FUNCTION_HPP

#include <ginac/ginac.h>
#include <iostream>

#include "product.hpp"
#include "sum.hpp"
#include "polynom.hpp"

product parse_power(const GiNaC::ex& expr);
product parse_mul(const GiNaC::ex& expr);
sum parse_sum(const GiNaC::ex& expr);

class transfer_function
{
    public:
        transfer_function(const GiNaC::ex& e);

        transfer_function(const polynom& num, const polynom& den);

        transfer_function low_frequency() const;
        transfer_function high_frequency() const;

        void pretty_print(std::ostream& stream, const std::string& prefix = std::string()) const;

        friend std::ostream& operator<<(std::ostream& stream, const transfer_function& tf);

    private:
        GiNaC::ex unit;
        polynom numerator;
        polynom denominator;
};

#endif // TRANSFER_FUNCTION_HPP
