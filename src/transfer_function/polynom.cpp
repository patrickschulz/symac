#include "polynom.hpp"

#include <algorithm>

polynom::polynom(const std::string& var) :
    variable(var)
{

}

void polynom::set_monom(const monom& m, unsigned int degree)
{
    monoms[degree] = m;
}

void polynom::add_sum(const sum& s, unsigned int degree)
{
    monoms[degree].sum_.add_sum(s);
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

monom polynom::get_monom(unsigned int degree) const
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
    for(unsigned int i = 0; i < monoms.size(); ++i)
    {
        if(monoms.find(i) != monoms.end())
        {
            res = res + GiNaC::pow(var, i) * get_monom(i).sum_.to_ginac();
        }
    }
    return res;
}

std::ostream& operator<<(std::ostream& stream, const polynom& p)
{
    for(unsigned int i = 0; i < p.monoms.size(); ++i)
    {
        if(p.monoms.find(i) != p.monoms.end())
        {
            if(i > 0)
            {
                stream << p.variable;
                if(i > 1)
                {
                    stream << "^" << i;
                }
                stream << " * ";
            }
            stream << p.get_monom(i).sum_;
            if(i != p.monoms.size() - 1)
            {
                stream << " + ";
            }
        }
    }
    return stream;
}
