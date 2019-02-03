#ifndef SUM_HPP
#define SUM_HPP

#include <ginac/ginac.h>
#include <vector>

#include "product.hpp"

class sum
{
    public:
        void add_sum(const sum& s);
        void add_product(const product& p);

        GiNaC::ex to_ginac() const;

        std::vector<product>::const_iterator begin() const;
        std::vector<product>::const_iterator end() const;

        friend std::ostream& operator<<(std::ostream& stream, const sum& s);

    private:
        std::vector<product> products;
};

#endif // SUM_HPP
