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
    GiNaC::symbol R1("R1");
    GiNaC::symbol R2("R2");
    GiNaC::symbol C1("C1");
    GiNaC::symbol C2("C2");
    GiNaC::ex e = (R1 + R2) * (C1 + C2 * (R1 * C1));
    
    debug_print(e);
    debug_print(e.expand());
    std::cout << e << '\n' << e.expand() << '\n';
}
