#ifndef PRODUCT_HPP
#define PRODUCT_HPP

#include <ginac/ginac.h>
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

        product& operator*(const product& p)
        {
            prefix = prefix * p.prefix;
            std::copy(p.elements.begin(), p.elements.end(), std::back_inserter(elements));
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& stream, const product& p);

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

#endif // PRODUCT_HPP
