#ifndef POLYNOM_HPP
#define POLYNOM_HPP

#include <ginac/ginac.h>

#include "sspace_symbols.hpp"
#include "product.hpp"

class polynom
{
    public:
        void add_product(const product& p, unsigned int degree)
        {
            vec.resize(degree + 1);
            vec[degree] = vec[degree] * p;
        }

        unsigned int degree()
        {
            return vec.size() - 1;
        }

        friend std::ostream& operator<<(std::ostream& stream, const polynom& p);

    private:
        std::vector<product> vec;
};

std::ostream& operator<<(std::ostream& stream, const polynom& p)
{
    for(unsigned int i = 0; i < p.vec.size(); ++i)
    {
        stream << p.vec[i];
        stream << " * s^" << i;
        if(i != p.vec.size() - 1)
        {
            stream << " + ";
        }
    }
    return stream;
}

#endif // POLYNOM_HPP
