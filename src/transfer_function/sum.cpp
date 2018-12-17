#include "sum.hpp"

void sum::add_product(const product& p)
{
    vec.push_back(p);
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