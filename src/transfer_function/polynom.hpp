#ifndef POLYNOM_HPP
#define POLYNOM_HPP

#include <ginac/ginac.h>
#include <map>

#include "sum.hpp"

struct monom
{
    sum sum_;
};

class polynom
{
    public:
        polynom(const std::string& var);

        void set_monom(const monom& m, unsigned int degree);

        void add_sum(const sum& s, unsigned int degree);

        unsigned int degree() const;
        bool exists(unsigned int degree) const;

        monom get_monom(unsigned int degree) const;

        polynom select_monoms(unsigned int degree) const;

        GiNaC::ex to_ginac(const GiNaC::symbol&) const;

        friend std::ostream& operator<<(std::ostream& stream, const polynom& p);

    private:
        std::string variable;
        std::map<unsigned int, monom> monoms;
};

#endif // POLYNOM_HPP
