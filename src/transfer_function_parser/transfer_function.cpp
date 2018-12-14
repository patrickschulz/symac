#include "transfer_function.hpp"

#include <vector>

#include "../symbol.hpp"

product parse_power(const GiNaC::ex& expr)
{
    product p;
    GiNaC::ex base = expr.op(0);
    int power = GiNaC::ex_to<GiNaC::numeric>(expr.op(1)).to_int();
    for(int i = 0; i < power; ++i)
    {
        p.add_factor(base);
    }
    return p;
}

product parse_mul(const GiNaC::ex& expr)
{
    product p;
    if(expr.nops() == 0)
    {
        p.add_factor(expr);
    }
    for(unsigned int i = 0; i < expr.nops(); ++i)
    {
        GiNaC::ex factor = expr.op(i);
        // check if the factor is a power
        if(GiNaC::is_exactly_a<GiNaC::power>(factor))
        {
            p = p * parse_power(factor);
        }
        else
        {
            p.add_factor(expr.op(i));
        }
    }
    return p;
}

sum parse_sum(const GiNaC::ex& expr)
{
    sum S;
    if(GiNaC::is_exactly_a<GiNaC::add>(expr))
    {
        for(unsigned int i = 0; i < expr.nops(); ++i)
        {
            product p = parse_mul(expr.op(i));
            S.add_product(p);
        }
    }
    else if(GiNaC::is_exactly_a<GiNaC::mul>(expr))
    {
        product p = parse_mul(expr);
        S.add_product(p);
    }
    else if(GiNaC::is_exactly_a<GiNaC::power>(expr))
    {
        product p = parse_power(expr);
        S.add_product(p);
    }
    else // atomic symbol/numeric
    {
        product p = parse_mul(expr);
        S.add_product(p);
    }
    return S;
}

transfer_function::transfer_function(const GiNaC::ex& e)
{
    GiNaC::ex numden = e.numer_denom();
    GiNaC::ex numex = numden[0].expand();
    GiNaC::ex denex = numden[1].expand();

    GiNaC::symbol s = get_symbol("s");

    for(int i = numex.ldegree(s); i <= numex.degree(s); ++i)
    {
        GiNaC::ex expr = numex.coeff(s, i);
        sum S = parse_sum(expr);
        numerator.add_sum(S, i);
    }
    for(int i = denex.ldegree(s); i <= denex.degree(s); ++i)
    {
        GiNaC::ex expr = denex.coeff(s, i);
        sum S = parse_sum(expr);
        denominator.add_sum(S, i);
    }
}

transfer_function::transfer_function(const polynom& num, const polynom& den) :
    numerator(num), denominator(den)
{ }

transfer_function transfer_function::low_frequency() const
{
    polynom num = numerator.select_monoms(0);
    polynom den = denominator.select_monoms(0);
    return transfer_function(num, den);
}

transfer_function transfer_function::high_frequency() const
{
    polynom num = numerator.select_monoms(numerator.degree());
    polynom den = denominator.select_monoms(denominator.degree());
    return transfer_function(num, den);
}

void transfer_function::pretty_print(std::ostream& stream, const std::string& prefix) const
{
    std::ostringstream stmp;
    stmp << GiNaC::dflt << numerator;
    unsigned int numsize = stmp.str().size();
    stmp.str("");
    stmp.clear();
    stmp << GiNaC::dflt << denominator;
    unsigned int densize = stmp.str().size();

    unsigned int numfill;
    unsigned int denfill;
    if(numsize > densize)
    {
        numfill = 0;
        denfill = (numsize - densize) / 2;
    }
    else
    {
        numfill = (densize - numsize) / 2;
        denfill = 0;
    }
    unsigned int offset = prefix.size();
    unsigned int rulefill = 4; // should be even
    stream << GiNaC::dflt;
    stream << std::string(numfill + offset + rulefill / 2, ' ') << numerator << '\n';
    stream << prefix << std::string(std::max(numsize, densize) + rulefill, '-') << '\n';
    stream << std::string(denfill + offset + rulefill / 2, ' ') << denominator << '\n';
    stream << '\n';
}

std::ostream& operator<<(std::ostream& stream, const transfer_function& tf)
{
    stream << '(' << tf.numerator << ')' << " / " << '(' << tf.denominator << ')';
    return stream;
}
