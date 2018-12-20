#ifndef PRODUCT_HPP
#define PRODUCT_HPP

#include <ginac/ginac.h>
#include <ginac/numeric.h>
#include <vector>

class product
{
    public:
        product();
        product(int i);

        void add_factor(const GiNaC::symbol& sym);
        void add_factor(const GiNaC::numeric& num);
        void add_factor(const GiNaC::ex& expr);

        GiNaC::ex to_ginac() const;

        GiNaC::numeric get_prefix() const;
        std::vector<GiNaC::symbol>::const_iterator begin() const;
        std::vector<GiNaC::symbol>::const_iterator end() const;

        friend std::ostream& operator<<(std::ostream& stream, const product& p);
        friend product operator*(const product& left, const product& right);

    private:
        GiNaC::numeric prefix;
        std::vector<GiNaC::symbol> elements;
};

#endif // PRODUCT_HPP
