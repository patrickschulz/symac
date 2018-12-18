#include "sum.hpp"

void sum::add_product(const product& p)
{
    vec.push_back(p);
}

GiNaC::ex sum::to_ginac() const
{
    GiNaC::ex res;
    for(const auto& e : vec)
    {
        res = res + e;
    }
    return res;
}

std::ostream& operator<<(std::ostream& stream, const sum& s)
{
    if(s.vec.size() > 1)
    {
        stream << '(';
    }
    for(unsigned int i = 0; i < s.vec.size(); ++i)
    {
        stream << s.vec[i];
        if(i != s.vec.size() - 1)
        {
            stream << " + ";
        }
    }
    if(s.vec.size() > 1)
    {
        stream << ')';
    }
    return stream;
}
