#ifndef QUANTITY_HPP
#define QUANTITY_HPP

#include <string>
#include <boost/fusion/include/adapt_struct.hpp>

struct quantity
{
    std::string function;
    std::string symbol;
};

BOOST_FUSION_ADAPT_STRUCT(
    quantity,
    (std::string, function)
    (std::string, symbol)
)


#endif // QUANTITY_HPP
