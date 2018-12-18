#include "polynom.hpp"

void polynom::set_monom(const monom& m, unsigned int degree)
{
    monoms.resize(degree + 1);
    monoms[degree] = m;
}

void polynom::add_sum(const sum& s, unsigned int degree)
{
    monoms.resize(degree + 1);
    monoms[degree].sum_.add_sum(s);
    monoms[degree].valid = true;
}

unsigned int polynom::degree() const
{
    return monoms.size() - 1;
}

monom polynom::get_monom(unsigned int degree) const
{
    return monoms[degree];
}

polynom polynom::select_monoms(unsigned int degree) const
{
    polynom pol;
    pol.set_monom(get_monom(degree), degree);
    return pol;
}

GiNaC::ex polynom::to_ginac(const GiNaC::symbol& var) const
{
    GiNaC::ex res;
    for(unsigned int i = 0; i < monoms.size(); ++i)
    {
        if(monoms[i].valid)
        {
            res = GiNaC::pow(var, i) * monoms[i].sum_.to_ginac();
        }
    }
    return res;
}

std::ostream& operator<<(std::ostream& stream, const polynom& p)
{
    for(unsigned int i = 0; i < p.monoms.size(); ++i)
    {
        if(p.monoms[i].valid)
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
            stream << p.monoms[i].sum_;
            if(i != p.monoms.size() - 1)
            {
                stream << " + ";
            }
        }
    }
    return stream;
}
