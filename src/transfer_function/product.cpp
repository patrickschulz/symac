#include "product.hpp"

#include <iostream>

product::product() :
    prefix(1)
{ }

product::product(int i) :
    prefix(GiNaC::numeric(i))
{ }

void product::add_factor(const GiNaC::symbol& sym)
{
    elements.push_back(sym);
}

void product::add_factor(const GiNaC::numeric& num)
{
    prefix = prefix * num;
}

void product::add_factor(const GiNaC::ex& expr)
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

std::ostream& operator<<(std::ostream& stream, const product& p)
{
    if(p.prefix != 1 || p.elements.size() == 0)
    {
        if(p.prefix != -1)
        {
            stream << p.prefix;
            if(p.elements.size() > 0)
            {
                stream << " * ";
            }
        }
        else
        {
            stream << "-";
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

product operator*(const product& left, const product& right)
{
    product res;
    res.prefix = left.prefix * right.prefix;
    std::copy(left.elements.begin(), left.elements.end(), std::back_inserter(res.elements));
    std::copy(right.elements.begin(), right.elements.end(), std::back_inserter(res.elements));
    return res;
}