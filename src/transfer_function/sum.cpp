#include "sum.hpp"

void sum::add_sum(const sum& s)
{
    for(const product& p : s.products)
    {
        add_product(p);
    }
}

void sum::add_product(const product& p)
{
    products.push_back(p);
}

GiNaC::ex sum::to_ginac() const
{
    GiNaC::ex res;
    for(const auto& e : products)
    {
        res = res + e.to_ginac();
    }
    return res;
}

std::vector<product>::const_iterator sum::begin() const
{
    return products.begin();
}

std::vector<product>::const_iterator sum::end() const
{
    return products.end();
}

std::ostream& operator<<(std::ostream& stream, const sum& s)
{
    if(s.products.size() > 1)
    {
        stream << '(';
    }
    for(unsigned int i = 0; i < s.products.size(); ++i)
    {
        stream << s.products[i];
        if(i != s.products.size() - 1)
        {
            stream << " + ";
        }
    }
    if(s.products.size() > 1)
    {
        stream << ')';
    }
    return stream;
}
