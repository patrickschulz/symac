#include "polynom.hpp"

void polynom::set_monom(const monom& m, unsigned int degree)
{
    vec.resize(degree + 1);
    vec[degree] = m;
}

void polynom::add_sum(const sum& s, unsigned int degree)
{
    vec.resize(degree + 1);
    vec[degree].sums.push_back(s);
    vec[degree].valid = true;
}

unsigned int polynom::degree() const
{
    return vec.size() - 1;
}

monom polynom::get_monom(unsigned int degree) const
{
    return vec[degree];
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
    for(unsigned int i = 0; i < vec.size(); ++i)
    {
        if(vec[i].valid)
        {
            res = var^i * vec[i].to_ginac();
        }
    }
    return res;
}

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
