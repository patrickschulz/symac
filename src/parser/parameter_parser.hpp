#ifndef PARAMETER_PARSER_HPP
#define PARAMETER_PARSER_HPP

#include "../component.hpp"

#include "parser_common.hpp"

struct parameter_parser_type : public qi::grammar<Iterator, Skipper_type, parameter_t()>
{
    parameter_parser_type();

    qi::rule<Iterator, std::string()> key, value;
    qi::rule<Iterator, Skipper_type, parameter_t()> main;
};

extern parameter_parser_type parameter_parser;

#endif // PARAMETER_PARSER_HPP
