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
        transfer_function();
        transfer_function(const GiNaC::ex& e);

        transfer_function(const polynom& num, const polynom& den);

        transfer_function low_frequency() const;
        transfer_function high_frequency() const;

        void set_prefix(const sum& s);
        void set_numerator(const polynom& num);
        void set_denominator(const polynom& den);
        sum get_prefix() const;
        polynom get_numerator() const;
        polynom get_denominator() const;

        GiNaC::ex gain() const;
        std::vector<GiNaC::ex> zeros() const;
        std::vector<GiNaC::ex> poles() const;

        GiNaC::ex integrate() const;

        GiNaC::ex to_ginac(const GiNaC::possymbol&) const;

        void pretty_print(std::ostream& stream, const std::string& prefix = std::string()) const;

        friend std::ostream& operator<<(std::ostream& stream, const transfer_function& tf);

    private:
        sum prefix;
        polynom numerator;
        polynom denominator;
};

#endif // TRANSFER_FUNCTION_HPP
