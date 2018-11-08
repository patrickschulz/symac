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

struct component_parser_type : public qi::grammar<std::string::iterator, qi::blank_type, component()>
{
    typedef std::string::iterator Iterator;

    component_parser_type() : component_parser_type::base_type(main, "component")
    {
        using qi::attr;
        using qi::alnum;
        using qi::char_;
        using qi::graph;
        using qi::repeat;
        using qi::eol;
        using qi::_r1;

        name = +graph;
        terminal = +(alnum | char_("-:_!"));
        value    = +(char_ - eol);
        terminals = repeat(_r1)[terminal];

        two_terminal_device   = two_terminal_identifier   >> name >> terminals(2) >> value;
        three_terminal_device = three_terminal_identifier >> name >> terminals(3) >> attr("OPDUMMY");
        four_terminal_device  = four_terminal_identifier  >> name >> terminals(4) >> value;

        main = two_terminal_device   |
               three_terminal_device |
               four_terminal_device;
    }

    qi::rule<Iterator, std::string()> name, terminal, value;
    qi::rule<Iterator, qi::blank_type, std::vector<std::string>(int)> terminals;
    qi::rule<Iterator, qi::blank_type, component_proxy()> two_terminal_device, three_terminal_device, four_terminal_device;
    qi::rule<Iterator, qi::blank_type, component()> main;
} component_parser;

#endif // COMPONENT_PARSER_HPP
