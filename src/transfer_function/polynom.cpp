#include "polynom.hpp"

#include "pretty.hpp"

#include <algorithm>

polynom::polynom(const std::string& var) :
    variable(var)
{

}

void polynom::set_monom(const sum& m, unsigned int degree)
{
    monoms[degree] = m;
}

void polynom::add_sum(const sum& s, unsigned int degree)
{
    monoms[degree].add_sum(s);
}

unsigned int polynom::degree() const
{
    return monoms.rbegin()->first;
}

bool polynom::exists(unsigned int degree) const
{
    auto cit = monoms.find(degree);
    return cit != monoms.end();
}

sum polynom::get_monom(unsigned int degree) const
{
    auto cit = monoms.find(degree);
    return cit->second;
}

polynom polynom::select_monoms(unsigned int degree) const
{
    polynom pol(variable);
    pol.set_monom(get_monom(degree), degree);
    return pol;
}

GiNaC::ex polynom::to_ginac(const GiNaC::symbol& var) const
{
    GiNaC::ex res;
    for(unsigned int i = 0; i <= degree(); ++i)
    {
        if(exists(i))
        {
            res = res + GiNaC::pow(var, i) * get_monom(i).to_ginac();
        }
    }
    return res;
}

std::ostream& operator<<(std::ostream& stream, const polynom& p)
{
    for(unsigned int i = 0; i <= p.degree(); ++i)
    {
        if(p.exists(i))
        {
            if(i > 0)
            {
                stream << p.variable;
                if(i > 1)
                {
                    stream << "^" << i;
                }
                stream << ' ' << multsign << ' ';
            }
            stream << p.get_monom(i);
            if(i != p.degree())
            {
                stream << " + ";
            }
        }
    }
    return stream;
}
