#ifndef NETLIST_PARSER_HPP
#define NETLIST_PARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>

#include "../comment.hpp"
#include "../subcircuit.hpp"
#include "../command.hpp"

namespace qi = boost::spirit::qi;

typedef boost::variant<component, command, comment, subcircuit, subcircuit_instance> netlist_attribute_type;
typedef std::string::iterator Iterator;
typedef qi::blank_type Skipper_type;
typedef qi::rule<Iterator, Skipper_type, netlist_attribute_type()>* netlist_parser_local;

struct netlist_parser_type : public qi::grammar<Iterator, Skipper_type, std::vector<netlist_attribute_type>()>
{
    netlist_parser_type();

    netlist_parser_local local;

    qi::rule<Iterator, Skipper_type, netlist_attribute_type()> spice, spectre;
    qi::rule<Iterator, Skipper_type, netlist_parser_local> switch_rule;
    qi::symbols<char, netlist_parser_local> switch_map;
    qi::rule<Iterator, Skipper_type, std::vector<netlist_attribute_type>()> main;
};

extern netlist_parser_type netlist_parser;

#endif // NETLIST_PARSER_HPP

// vim: nowrap
