#ifndef COMPONENT_PARSER_HPP
#define COMPONENT_PARSER_HPP

#include <vector>
#include <string>
#include <map>

#include <ginac/ginac.h>

#include <boost/fusion/include/vector.hpp>

#include "parser_common.hpp"

struct component_parser_type : public qi::grammar<Iterator, Skipper_type, component()>
{
    component_parser_type();

    qi::rule<Iterator, std::string()> name, terminal, value;
    qi::rule<Iterator, Skipper_type, std::vector<parameter_t>()> parameters;
    qi::rule<Iterator, Skipper_type, std::vector<std::string>(int)> terminals;
    qi::rule<Iterator, Skipper_type, spice_component_proxy()> two_terminal_device, three_terminal_device, four_terminal_device, port;
    qi::rule<Iterator, Skipper_type, component()> main;
};

extern component_parser_type component_parser;

#endif // COMPONENT_PARSER_HPP
