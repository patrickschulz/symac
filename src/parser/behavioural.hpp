#ifndef BEHAVIOURAL_PARSER_HPP
#define BEHAVIOURAL_PARSER_HPP

#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/vector.hpp>

#include "../behavioural.hpp"
#include "parser_common.hpp"
#include "expression_parser.hpp"
#include "../quantity.hpp"

BOOST_FUSION_ADAPT_STRUCT(
    behavioural,
    (std::string, name)
    (std::vector<std::string>, terminals)
    (std::vector<ast::expression<quantity>>, expressions)
)

struct behavioural_parser_type : public qi::grammar<Iterator, Skipper_type, behavioural()>
{
    behavioural_parser_type();

    qi::rule<Iterator, std::string()> name, terminal;
    qi::rule<Iterator, Skipper_type, std::vector<std::string>()> terminals;
    qi::rule<Iterator, Skipper_type, ast::expression<quantity>()> bodyline;
    qi::rule<Iterator, Skipper_type, std::vector<ast::expression<quantity>>()> body;

    // expression rules
    qi::rule<Iterator, std::string()> function = +qi::alpha;
    qi::rule<Iterator, std::string()> symbol = +(qi::alnum | qi::char_(" ,."));
    qi::rule<Iterator, std::string()> number = +qi::digit >> -(qi::char_(".") >> +qi::digit);
    qi::rule<Iterator, quantity(), Skipper_type> identifier = (function >> "(" >> symbol >> ")") | (qi::attr("NUMBER") >> number);
    symbolic_expression_type<quantity> symbolic_expression;

    qi::rule<Iterator, Skipper_type, behavioural()> main;
};

extern behavioural_parser_type behavioural_parser;

#endif // BEHAVIOURAL_PARSER_HPP
