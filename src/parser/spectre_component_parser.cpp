#include "spectre_component_parser.hpp"

struct spectre_identifier_type : qi::symbols<char, component_types>
{
    spectre_identifier_type()
    {
        add
            ("resistor",  ct_resistor)
            ("capacitor", ct_capacitor)
            ("inductor",  ct_inductor)
            ("vsource",   ct_voltage_source)
            ("isource",   ct_current_source)
            ("vcvs",      ct_voltage_controlled_voltage_source)
            ("ccvs",      ct_current_controlled_voltage_source)
            ("vccs",      ct_voltage_controlled_current_source)
            ("cccs",      ct_current_controlled_current_source)
        ;
    }
} spectre_identifier;

spectre_component_parser_type::spectre_component_parser_type() : spectre_component_parser_type::base_type(main, "component")
{
    using qi::alnum;
    using qi::alpha;
    using qi::char_;
    using qi::eol;
    using qi::lit;

    name = alpha >> *alnum;
    terminal = +(alnum | char_("-:_!"));
    value    = +(char_ - eol);
    terminals = lit("(") >> +terminal >> lit(")");

    device = name >> terminals >> spectre_identifier >> value;
    main = device;
}
