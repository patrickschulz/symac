#ifndef POLYNOM_HPP
#define POLYNOM_HPP

#include <ginac/ginac.h>

#include "sum.hpp"

struct monom
{
    monom() : valid(false)
    { }

    //monom(

    std::vector<sum> sums;
    bool valid;
};

class polynom
{
    public:
        void set_monom(const monom& m, unsigned int degree)
        {
            vec.resize(degree + 1);
            vec[degree] = m;
        }

        void add_sum(const sum& s, unsigned int degree)
        {
            vec.resize(degree + 1);
            vec[degree].sums.push_back(s);
            vec[degree].valid = true;
        }

        unsigned int degree()
        {
            return vec.size() - 1;
        }

        monom get_monom(unsigned int degree)
        {
            return vec[degree];
        }

        polynom select_monoms(unsigned int degree)
        {
            polynom pol;
            pol.set_monom(get_monom(degree), degree);
            return pol;
        }

        friend std::ostream& operator<<(std::ostream& stream, const polynom& p);

    private:
        std::vector<monom> vec;
};

std::ostream& operator<<(std::ostream& stream, const polynom& p)
{
    for(unsigned int i = 0; i < p.vec.size(); ++i)
    {
        if(p.vec[i].valid)
        {
            if(i > 0)
            {
                if(i > 1)
                {
                    stream << "s^" << i;
                }
                else
                {
                    stream << "s";
                }
                stream << " * ";
            }
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
