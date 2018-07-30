#ifndef TRANSFER_FUNCTION_HPP
#define TRANSFER_FUNCTION_HPP

#include <ginac/ginac.h>
#include <iostream>
#include <vector>

#include "sspace_symbols.hpp"
#include "util.hpp"

class transfer_function
{
    public:
        transfer_function(const GiNaC::ex& e)
        {
            GiNaC::ex numex = e.numer();
            GiNaC::ex denex = e.denom();

            for(int i = numex.ldegree(s); i <= numex.degree(s); ++i)
            {
                GiNaC::ex expr = numex.coeff(s, i);
                product p;
                if(expr.nops() == 0)
                {
                    p.add_factor(expr);
                }
                else
                {
                    for(unsigned int j = 0; j < expr.nops(); ++j)
                    {
                        p.add_factor(expr.op(j));
                    }
                }
                numerator.add_product(p, i);
            }
            for(int i = denex.ldegree(s); i <= denex.degree(s); ++i)
            {
                GiNaC::ex expr = denex.coeff(s, i);
                product p;
                if(expr.nops() == 0)
                {
                    p.add_factor(expr);
                }
                else
                {
                    for(unsigned int j = 0; j < expr.nops(); ++j)
                    {
                        p.add_factor(expr.op(j));
                    }
                }
                denominator.add_product(p, i);
            }
        }

        friend std::ostream& operator<<(std::ostream& stream, const transfer_function& tf);

    private:
        polynom numerator;
        polynom denominator;
};

std::ostream& operator<<(std::ostream& stream, const transfer_function& tf)
{
    stream << tf.numerator << " / " << tf.denominator;
    return stream;
}

#endif // TRANSFER_FUNCTION_HPP
