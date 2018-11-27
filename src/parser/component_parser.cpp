#include "component_parser.hpp"

struct two_terminal_identifier_type : qi::symbols<char, component_types>
{
    two_terminal_identifier_type()
    {
        add
            ("R", ct_resistor)
            ("C", ct_capacitor)
            ("L", ct_inductor)
            ("V", ct_voltage_source)
            ("I", ct_current_source)
        ;
    }
} two_terminal_identifier;

struct three_terminal_identifier_type : qi::symbols<char, component_types>
{
    three_terminal_identifier_type()
    {
        add
            ("O", ct_opamp)
        ;
    }
} three_terminal_identifier;

struct four_terminal_identifier_type : qi::symbols<char, component_types>
{
    four_terminal_identifier_type()
    {
        add
            ("E", ct_voltage_controlled_voltage_source)
            ("F", ct_current_controlled_voltage_source)
            ("G", ct_voltage_controlled_current_source)
            ("H", ct_current_controlled_current_source)
        ;
    }
} four_terminal_identifier;

struct port_identifier_type : qi::symbols<char, component_types>
{
    port_identifier_type()
    {
        add
            ("P", ct_port)
        ;
    }
} port_identifier;

component_parser_type::component_parser_type() : component_parser_type::base_type(main, "component")
{
    using qi::attr;
    using qi::alnum;
    using qi::char_;
    using qi::repeat;
    using qi::eol;
    using qi::_r1;
    using qi::no_skip;

    name = +alnum;
    terminal = +(alnum | char_("-:_!"));
    value    = +(char_ - eol);
    terminals = repeat(_r1)[terminal];

    two_terminal_device   = two_terminal_identifier   >> no_skip[name] >> terminals(2) >> value;
    three_terminal_device = three_terminal_identifier >> no_skip[name] >> terminals(3) >> attr("OPDUMMY");
    four_terminal_device  = four_terminal_identifier  >> no_skip[name] >> terminals(4) >> value;
    port                  = port_identifier           >> no_skip[name] >> terminals(2) >> attr("PORTDUMMY");

    main = two_terminal_device   |
           three_terminal_device |
           four_terminal_device  |
           port                  ;
}

component_parser_type component_parser;
