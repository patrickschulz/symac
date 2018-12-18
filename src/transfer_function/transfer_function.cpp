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

transfer_function::transfer_function(const GiNaC::ex& e) :
    numerator("s"), denominator("s")
{
    GiNaC::ex numden = e.numer_denom();
    GiNaC::ex numex = numden[0].expand();
    GiNaC::ex denex = numden[1].expand();

    GiNaC::symbol s = get_symbol("s");

    prefix = parse_sum(numex.unit(s) * numex.content(s));

    numex = numex.primpart(s);
    for(int i = numex.ldegree(s); i <= numex.degree(s); ++i)
    {
        GiNaC::ex expr = numex.coeff(s, i);
        sum S = parse_sum(expr.expand());
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

unsigned int get_output_size(const GiNaC::ex& e)
{
    std::ostringstream stmp;
    stmp << GiNaC::dflt << e;
    return stmp.str().size();
}
unsigned int get_output_size(const polynom& p)
{
    std::ostringstream stmp;
    stmp << GiNaC::dflt << p;
    return stmp.str().size();
}
unsigned int get_output_size(const sum& s)
{
    std::ostringstream stmp;
    stmp << GiNaC::dflt << s;
    return stmp.str().size();
}

GiNaC::ex transfer_function::to_ginac(const GiNaC::symbol& s) const
{
    GiNaC::ex num = numerator.to_ginac(s);
    GiNaC::ex den = denominator.to_ginac(s);
    GiNaC::ex pre = prefix.to_ginac();
    return pre* num / den;
}

void transfer_function::pretty_print(std::ostream& stream, const std::string& strpre) const
{
    unsigned int numsize = get_output_size(numerator);
    unsigned int densize = get_output_size(denominator);

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
    unsigned int offset = strpre.size() + get_output_size(prefix) + 3; // +3 for the " * " after the prefix part
    unsigned int rulefill = 2;
    stream << GiNaC::dflt;
    // print numerator
    stream << std::string(numfill + offset + rulefill, ' ') << numerator << '\n';
    // print strpre, prefix part and fraction rule
    stream << strpre << prefix << " * " << std::string(std::max(numsize, densize) + 2 * rulefill, '-') << '\n';
    // print denominator
    stream << std::string(denfill + offset + rulefill, ' ') << denominator << '\n';
    stream << '\n';
}

std::ostream& operator<<(std::ostream& stream, const transfer_function& tf)
{
    stream << '(' << tf.numerator << ')' << " / " << '(' << tf.denominator << ')';
    return stream;
}
