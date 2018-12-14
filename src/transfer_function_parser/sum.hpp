#ifndef SUM_HPP
#define SUM_HPP

#include <ginac/ginac.h>

#include "product.hpp"

class sum
{
    public:
        void add_product(const product& p)
        {
            vec.push_back(p);
        }

        friend std::ostream& operator<<(std::ostream& stream, const sum& s);

    private:
        std::vector<product> vec;
};

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

#endif // SUM_HPP
