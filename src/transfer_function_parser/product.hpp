#ifndef PRODUCT_HPP
#define PRODUCT_HPP

#include <ginac/ginac.h>
#include <ginac/numeric.h>
#include <iostream>
#include <vector>

#include "sspace_symbols.hpp"

class product
{
    public:
        product() :
            prefix(1)
        {

        }

        void add_factor(const GiNaC::symbol& sym)
        {
            elements.push_back(sym);
        }

        void add_factor(const GiNaC::numeric& num)
        {
            prefix = prefix * num;
        }

        void add_factor(const GiNaC::ex& expr)
        {
            if(GiNaC::is_exactly_a<GiNaC::numeric>(expr))
            {
                add_factor(GiNaC::ex_to<GiNaC::numeric>(expr));
            }
            else
            {
                add_factor(GiNaC::ex_to<GiNaC::symbol>(expr));
            }
        }

        friend std::ostream& operator<<(std::ostream& stream, const product& p);
        friend product operator*(product left, product right);

    private:
        GiNaC::numeric prefix;
        std::vector<GiNaC::symbol> elements;
};

std::ostream& operator<<(std::ostream& stream, const product& p)
{
    if(p.prefix != 1 || p.elements.size() == 0)
    {
        stream << p.prefix;
        if(p.elements.size() > 0)
        {
            stream << " * ";
        }
    }
    for(unsigned int i = 0; i < p.elements.size(); ++i)
    {
        stream << p.elements[i];
        if(i != p.elements.size() - 1)
        {
            stream << " * ";
        }
    }
    return stream;
}

product operator*(product left, product right)
{
    product res;
    res.prefix = left.prefix * right.prefix;
    std::copy(left.elements.begin(), left.elements.end(), std::back_inserter(res.elements));
    std::copy(right.elements.begin(), right.elements.end(), std::back_inserter(res.elements));
    return res;
}

#endif // PRODUCT_HPP
