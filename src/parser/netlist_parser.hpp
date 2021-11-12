#ifndef NETLIST_PARSER_HPP
#define NETLIST_PARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>

#include "../comment.hpp"
#include "../subcircuit.hpp"
#include "../command.hpp"
#include "../behavioural.hpp"

#include "parser_common.hpp"

#include "behavioural.hpp"
#include "command_parser.hpp"
#include "comment_parser.hpp"
#include "component_parser.hpp"
#include "parameter_parser.hpp"
#include "spectre_component_parser.hpp"
#include "subcircuit_parser.hpp"

namespace qi = boost::spirit::qi;

typedef boost::variant<component, command, comment, behavioural, subcircuit, subcircuit_instance> netlist_attribute_type;
typedef qi::rule<Iterator, Skipper_type, netlist_attribute_type()>* netlist_parser_local;

struct netlist_parser_type : public qi::grammar<Iterator, Skipper_type, std::vector<netlist_attribute_type>()>
{
    netlist_parser_type();

    netlist_parser_local local;

    qi::rule<Iterator, Skipper_type, netlist_attribute_type()> spice, spectre;
    qi::rule<Iterator, Skipper_type, netlist_parser_local> switch_rule;
    qi::symbols<char, netlist_parser_local> switch_map;
    qi::rule<Iterator, Skipper_type, std::vector<netlist_attribute_type>()> main;

    qi::rule<Iterator, Skipper_type> simplify;

    behavioural_parser_type behavioural_parser;
    command_parser_type command_parser;
    comment_parser_type comment_parser;
    component_parser_type component_parser;
    parameter_parser_type parameter_parser;
    spectre_component_parser_type spectre_component_parser;
    subcircuit_parser_type subcircuit_parser;
    subcircuit_instance_parser_type subcircuit_instance_parser;
};

#endif // NETLIST_PARSER_HPP

// vim: nowrap
