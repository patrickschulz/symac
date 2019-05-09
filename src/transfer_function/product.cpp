#include "product.hpp"

#include "pretty.hpp"

#include <iostream>

product::product() :
    prefix(1)
{ }

product::product(int i) :
    prefix(GiNaC::numeric(i))
{ }

void product::add_factor(const GiNaC::possymbol& sym)
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
        add_factor(GiNaC::ex_to<GiNaC::possymbol>(expr));
    }
}

GiNaC::ex product::to_ginac() const
{
    GiNaC::ex res = prefix;
    for(const auto& e: elements)
    {
        res = res * e;
    }
    return res;
}

GiNaC::numeric product::get_prefix() const
{
    return prefix;
}

std::vector<GiNaC::possymbol>::const_iterator product::begin() const
{
    return elements.begin();
}

std::vector<GiNaC::possymbol>::const_iterator product::end() const
{
    return elements.end();
}

std::ostream& operator<<(std::ostream& stream, const product& p)
{
    if(p.prefix != 1 || p.elements.size() == 0)
    {
        /*
        if(p.prefix != -1)
        {
            stream << p.prefix;
            if(p.elements.size() > 0)
            {
                stream << ' ' << multsign << ' ';
            }
        }
        else
        {
            stream << "-";
        }
        */
        stream << p.prefix;
        if(p.elements.size() > 0)
        {
            stream << ' ' << multsign << ' ';
        }
    }
    for(unsigned int i = 0; i < p.elements.size(); ++i)
    {
        stream << p.elements[i];
        if(i != p.elements.size() - 1)
        {
            stream << ' ' << multsign << ' ';
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
