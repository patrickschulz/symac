#include <iostream>

#include "sum.hpp"
#include "product.hpp"
#include "polynom.hpp"
#include "transfer_function.hpp"

#include "sspace_symbols.hpp"

void ginacprint(const GiNaC::symbol& sym)
{
    std::cerr << sym << '\n';
}

void ginacprint(const GiNaC::numeric& num)
{
    std::cerr << num << '\n';
}

class my_visitor : 
    public GiNaC::visitor,          // this is required
    public GiNaC::add::visitor,     // visit add objects
    public GiNaC::mul::visitor,     // visit mul objects
    public GiNaC::power::visitor,   // visit power objects
    public GiNaC::numeric::visitor, // visit numeric objects
    public GiNaC::symbol::visitor,  // visit sumbol objects
    public GiNaC::basic::visitor    // visit basic objects
{
    void visit(const GiNaC::add & x)
    { 
        std::cout << "called with an add object" << '\n';
    }

    void visit(const GiNaC::mul & x)
    { 
        std::cout << "called with an mul object" << '\n';
    }

    void visit(const GiNaC::power & x)
    { 
        std::cout << "called with an power object" << '\n';
    }

    void visit(const GiNaC::numeric & x)
    { 
        std::cout << "called with a numeric object" << '\n';
    }

    void visit(const GiNaC::symbol & x)
    { 
        std::cout << "called with a symbol object" << '\n';
    }

    void visit(const GiNaC::basic & x)
    { 
        std::cout << "called with a basic object" << '\n';
    }
};

GiNaC::symbol s = GiNaC::symbol("s");

int main()
{
    GiNaC::symbol x("x");
    GiNaC::symbol y("y");
    GiNaC::ex e = x * y + x + y;

    my_visitor v;

    for(GiNaC::const_preorder_iterator i = e.preorder_begin(); i != e.preorder_end(); ++i)
    {
        i->accept(v);
    }
    /*
    GiNaC::symbol R1 = GiNaC::symbol("R1");
    GiNaC::symbol R2 = GiNaC::symbol("R2");
    GiNaC::symbol C1 = GiNaC::symbol("C1");
    GiNaC::symbol C2 = GiNaC::symbol("C2");

    GiNaC::ex den1 = s * R1;
    GiNaC::ex den2 = s * R1 * C1;
    GiNaC::ex den3 = s * (R1 + R2);
    GiNaC::ex den4 = s * (R1 * C1 + R2 * C2);
    GiNaC::ex den5 = s * ((R1 + R2) * C1);

    debug_print(den1.coeff(s, 1));
    debug_print(den2.coeff(s, 1));
    debug_print(den3.coeff(s, 1));
    debug_print(den4.coeff(s, 1));
    debug_print(den5.coeff(s, 1));
    */
}
