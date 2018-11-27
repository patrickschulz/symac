#include "netlist_parser.hpp"

#include "component_parser.hpp"
#include "command_parser.hpp"
#include "comment_parser.hpp"
#include "subcircuit_parser.hpp"


netlist_parser_type::netlist_parser_type() : netlist_parser_type::base_type(main, "netlist")
{
    line = component_parser             | 
           command_parser               |  
           comment_parser               | 
           subcircuit_parser            | 
           subcircuit_instance_parser;

    main = -line % qi::eol >> qi::eoi;
}

netlist_parser_type netlist_parser;
