#ifndef COMPONENT_PARSER_HPP
#define COMPONENT_PARSER_HPP

#include <vector>
#include <string>
#include <map>

#include <ginac/ginac.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/vector.hpp>

#include "../component.hpp"

namespace qi = boost::spirit::qi;

BOOST_FUSION_ADAPT_STRUCT(
    component_proxy,
    (component_types, type)
    (std::string, name)
    (std::vector<std::string>, nodes)
    (std::string, value)
)

struct component_parser_type : public qi::grammar<std::string::iterator, qi::blank_type, component()>
{
    typedef std::string::iterator Iterator;

    component_parser_type();

    qi::rule<Iterator, std::string()> name, terminal, value;
    qi::rule<Iterator, qi::blank_type, std::vector<std::string>(int)> terminals;
    qi::rule<Iterator, qi::blank_type, component_proxy()> two_terminal_device, three_terminal_device, four_terminal_device, port;
    qi::rule<Iterator, qi::blank_type, component()> main;
};

extern component_parser_type component_parser;

#endif // COMPONENT_PARSER_HPP
