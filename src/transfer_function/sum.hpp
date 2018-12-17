#ifndef SUM_HPP
#define SUM_HPP

#include <ginac/ginac.h>
#include <vector>

#include "product.hpp"

class sum
{
    public:
        void add_product(const product& p);

        friend std::ostream& operator<<(std::ostream& stream, const sum& s);

    private:
        std::vector<product> vec;
};

#endif // SUM_HPP
