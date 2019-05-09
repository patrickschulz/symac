#ifndef BEHAVIOURAL_HPP
#define BEHAVIOURAL_HPP

#include <vector>
#include <string>
#include <ginac/ginac.h>

#include "parser/expression_parser.hpp"
#include "quantity.hpp"

class behavioural
{
    public:

    private:
    public:
        std::string name;
        std::vector<std::string> terminals;
        std::vector<ast::expression<quantity>> expressions;
};

#endif //BEHAVIOURAL_HPP
