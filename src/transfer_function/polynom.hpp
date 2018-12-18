#ifndef POLYNOM_HPP
#define POLYNOM_HPP

#include <ginac/ginac.h>
#include <vector>

#include "sum.hpp"

struct monom
{
    monom() : valid(false)
    { }

    std::vector<sum> sums;
    bool valid;
};

class polynom
{
    public:
        void set_monom(const monom& m, unsigned int degree);

        void add_sum(const sum& s, unsigned int degree);

        unsigned int degree() const;

        monom get_monom(unsigned int degree) const;

        polynom select_monoms(unsigned int degree) const;

        GiNaC::ex to_ginac(const GiNaC::symbol&) const;

        friend std::ostream& operator<<(std::ostream& stream, const polynom& p);

    private:
        std::vector<monom> vec;
};

#endif // POLYNOM_HPP
