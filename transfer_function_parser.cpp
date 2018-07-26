#include <ginac/ginac.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

GiNaC::symbol s = GiNaC::symbol("s");

void debug_print_internal(const GiNaC::ex& e)
{
    std::cout << GiNaC::ex_to<GiNaC::basic>(e).class_name();
    std::cout << "(";
    size_t n = e.nops();
    if (n)
    {
        for (size_t i=0; i<n; i++)
        {
            debug_print_internal(e.op(i));
            if (i != n-1)
            {
                std::cout << ",";
            }
        }
    }
    else
    {
        std::cout << e;
    }
    std::cout << ")";
}

void debug_print(const GiNaC::ex & e)
{
    debug_print_internal(e);
    std::cout << '\n';
}

std::string get_ginac_type(const GiNaC::ex& e)
{
    std::string type = GiNaC::ex_to<GiNaC::basic>(e).class_name();
    return type;
}

class product
{
    public:
        product() :
            prefix(1)
        {

        }

        void add_factor(const GiNaC::symbol& sym)
        {
            elements.push_back(sym);
        }

        void add_factor(const GiNaC::numeric& num)
        {
            prefix = prefix * num;
        }

        void print()
        {
            if(prefix != 1 || elements.size() == 0)
            {
                std::cout << prefix;
                if(elements.size() > 0)
                {
                    std::cout << " * ";
                }
            }
            for(int i = 0; i < elements.size(); ++i)
            {
                std::cout << elements[i];
                if(i != elements.size() - 1)
                {
                    std::cout << " * ";
                }
            }
        }
    private:
        GiNaC::numeric prefix;
        std::vector<GiNaC::symbol> elements;
};

class polynom
{
    public:
        void add_product(const product& p, unsigned int degree)
        {
            vec.resize(degree);
            vec.push_back(p);
        }

        void print()
        {
            for(int i = 0; i < vec.size(); ++i)
            {
                vec[i].print();
                std::cout << " * s^" << i;
                if(i != vec.size() - 1)
                {
                    std::cout << " + ";
                }
            }
            std::cout << '\n';
        }

    private:
        std::vector<product> vec;
};

class transfer_function
{
    public:
        transfer_function(const GiNaC::ex& e)
        {
            GiNaC::ex numex = e.numer();
            GiNaC::ex denex = e.denom();

            for(int i = numex.ldegree(s); i <= numex.degree(s); ++i)
            {
                GiNaC::ex expr = numex.coeff(s, i);
                product p;
                for(int j = 0; j < expr.nops(); ++j)
                {
                    if(GiNaC::is_exactly_a<GiNaC::numeric>(expr.op(j)))
                    {
                        p.add_factor(GiNaC::ex_to<GiNaC::numeric>(expr.op(j)));
                    }
                    else
                    {
                        p.add_factor(GiNaC::ex_to<GiNaC::symbol>(expr.op(j)));
                    }
                }
                numerator.add_product(p, i);
            }
            for(int i = denex.ldegree(s); i <= denex.degree(s); ++i)
            {
                GiNaC::ex expr = denex.coeff(s, i);
                product p;
                for(int j = 0; j < expr.nops(); ++j)
                {
                    if(GiNaC::is_exactly_a<GiNaC::numeric>(expr.op(j)))
                    {
                        p.add_factor(GiNaC::ex_to<GiNaC::numeric>(expr.op(j)));
                    }
                    else
                    {
                        p.add_factor(GiNaC::ex_to<GiNaC::symbol>(expr.op(j)));
                    }
                }
                denominator.add_product(p, i);
            }

            numerator.print();
            denominator.print();
        }

    private:
        polynom numerator;
        polynom denominator;
};

class tf_visitor
 : public GiNaC::visitor,          // this is required
   public GiNaC::add::visitor,     // visit add objects
   public GiNaC::mul::visitor,     // visit mul objects
   public GiNaC::power::visitor,   // visit power objects
   public GiNaC::numeric::visitor, // visit numeric objects
   public GiNaC::symbol::visitor,  // visit symbol objects
   public GiNaC::basic::visitor    // visit basic objects
{
    public:
        void visit(const GiNaC::add & x)
        { std::cout << "called with an add object" << std::endl; }

        void visit(const GiNaC::mul & x)
        { std::cout << "called with an mul object" << std::endl; }

        void visit(const GiNaC::power & x)
        { std::cout << "called with an power object" << std::endl; }

        void visit(const GiNaC::numeric & x)
        { std::cout << "called with a numeric object" << std::endl; }

        void visit(const GiNaC::symbol & x)
        { std::cout << "called with a symbol object" << std::endl; }

        void visit(const GiNaC::basic & x)
        { std::cout << "called with a basic object" << std::endl; }
};

int main()
{
    GiNaC::symbol R1 = GiNaC::symbol("R1");
    GiNaC::symbol R2 = GiNaC::symbol("R2");
    GiNaC::symbol C1 = GiNaC::symbol("C1");
    GiNaC::symbol C2 = GiNaC::symbol("C2");

    GiNaC::ex tfex = ((1 + s * R1 * C1) / (1 + s * R1 * C2 + s * s * R1 * R2 * C1 * C2));

    transfer_function tf(tfex);
}
