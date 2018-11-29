#include "netlist_parser.hpp"

#include "component_parser.hpp"
#include "command_parser.hpp"
#include "comment_parser.hpp"
#include "subcircuit_parser.hpp"

#include <iostream>

void simulator(const std::string& s)
{
    std::cout << s << '\n';
}

netlist_parser_type::netlist_parser_type() : netlist_parser_type::base_type(main, "netlist")
{
    line = qi::omit[language]             |
           component_parser               | 
           command_parser                 |  
           comment_parser                 | 
           subcircuit_parser              | 
           subcircuit_instance_parser;

    //language = qi::lit("simulator") >> qi::lit("lang") >> qi::lit("=") >> simulator_string[&simulator];

    language.add
        ("spectre", &spectre_line)
        ("spice", &spice_line)
        ;

    main = -line % qi::eol >> qi::eoi;
}

netlist_parser_type netlist_parser;
