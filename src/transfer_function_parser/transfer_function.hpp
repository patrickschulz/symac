#ifndef TRANSFER_FUNCTION_HPP
#define TRANSFER_FUNCTION_HPP

#include <ginac/ginac.h>
#include <iostream>
#include <vector>

#include "sspace_symbols.hpp"
#include "util.hpp"

product parse_power(const GiNaC::ex& expr)
{
    product p;
    GiNaC::ex base = expr.op(0);
    int power = GiNaC::ex_to<GiNaC::numeric>(expr.op(1)).to_int();
    for(int i = 0; i < power; ++i)
    {
        p.add_factor(base);
    }
    return p;
}

product parse_mul(const GiNaC::ex& expr)
{
    product p;
    if(expr.nops() == 0)
    {
        p.add_factor(expr);
    }
    for(unsigned int i = 0; i < expr.nops(); ++i)
    {
        GiNaC::ex factor = expr.op(i);
        // check if the factor is a power
        if(GiNaC::is_exactly_a<GiNaC::power>(factor))
        {
            p = p * parse_power(factor);
        }
        else
        {
            p.add_factor(expr.op(i));
        }
    }
    return p;
}

sum parse_sum(const GiNaC::ex& expr)
{
    sum S;
    if(GiNaC::is_exactly_a<GiNaC::add>(expr))
    {
        for(unsigned int i = 0; i < expr.nops(); ++i)
        {
            product p = parse_mul(expr.op(i));
            S.add_product(p);
        }
    }
    else if(GiNaC::is_exactly_a<GiNaC::mul>(expr))
    {
        product p = parse_mul(expr);
        S.add_product(p);
    }
    else if(GiNaC::is_exactly_a<GiNaC::power>(expr))
    {
        product p = parse_power(expr);
        S.add_product(p);
    }
    else // atomic symbol/numeric
    {
        product p = parse_mul(expr);
        S.add_product(p);
    }
    return S;
}

class transfer_function
{
    public:
        transfer_function(const GiNaC::ex& e)
        {
            GiNaC::ex numden = e.numer_denom();
            GiNaC::ex numex = numden[0].expand();
            GiNaC::ex denex = numden[1].expand();

            for(int i = numex.ldegree(s); i <= numex.degree(s); ++i)
            {
                GiNaC::ex expr = numex.coeff(s, i);
                sum S = parse_sum(expr);
                numerator.add_sum(S, i);
            }
            for(int i = denex.ldegree(s); i <= denex.degree(s); ++i)
            {
                GiNaC::ex expr = denex.coeff(s, i);
                sum S = parse_sum(expr);
                denominator.add_sum(S, i);
            }
        }

        friend std::ostream& operator<<(std::ostream& stream, const transfer_function& tf);

    private:
        polynom numerator;
        polynom denominator;
};

std::ostream& operator<<(std::ostream& stream, const transfer_function& tf)
{
    stream << '(' << tf.numerator << ')' << " / " << '(' << tf.denominator << ')';
    return stream;
}

#endif // TRANSFER_FUNCTION_HPP
