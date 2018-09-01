#ifndef POLYNOM_HPP
#define POLYNOM_HPP

#include <ginac/ginac.h>

#include "sspace_symbols.hpp"
#include "sum.hpp"

struct polelement
{
    std::vector<sum> sums;
    bool valid;
};

class polynom
{
    public:
        void add_sum(const sum& p, unsigned int degree)
        {
            vec.resize(degree + 1);
            vec[degree].sums.push_back(p);
            vec[degree].valid = true;
        }

        unsigned int degree()
        {
            return vec.size() - 1;
        }

        friend std::ostream& operator<<(std::ostream& stream, const polynom& p);

    private:
        std::vector<polelement> vec;
};

std::ostream& operator<<(std::ostream& stream, const polynom& p)
{
    for(unsigned int i = 0; i < p.vec.size(); ++i)
    {
        if(p.vec[i].valid)
        {
            stream << "s^" << i << " * ";
            if(p.vec[i].sums.size() > 1)
            {
                stream << '(';
            }
            for(unsigned int j = 0; j < p.vec[i].sums.size(); ++j)
            {
                stream << p.vec[i].sums[j];
                if(j != p.vec[i].sums.size() - 1)
                {
                    stream << " + ";
                }
            }
            if(p.vec[i].sums.size() > 1)
            {
                stream << ')';
            }
            if(i != p.vec.size() - 1)
            {
                stream << " + ";
            }
        }
    }
    return stream;
}

#endif // POLYNOM_HPP
