#ifndef NETLIST_PARSER_HPP
#define NETLIST_PARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>

#include "component_parser.hpp"
#include "command_parser.hpp"
#include "comment_parser.hpp"
//#include "subcircuit_parser.hpp"

namespace qi = boost::spirit::qi;

typedef boost::variant<component_proxy, command, comment> netlist_attribute_type;
struct netlist_parser_type : public qi::grammar<std::string::iterator, qi::ascii::blank_type, std::vector<netlist_attribute_type>()>
{
    typedef std::string::iterator Iterator;

    netlist_parser_type() : netlist_parser_type::base_type(main, "netlist")
    {
        line = component_parser | command_parser | comment_parser;
        main = (-line % qi::eol) >> qi::eoi;
    }

    qi::rule<Iterator, qi::ascii::blank_type, netlist_attribute_type()> line;
    qi::rule<Iterator, qi::ascii::blank_type, std::vector<netlist_attribute_type>()> main;
} netlist_parser;

#endif // NETLIST_PARSER_HPP
