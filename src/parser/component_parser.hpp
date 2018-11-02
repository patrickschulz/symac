#ifndef COMPONENT_PARSER_HPP
#define COMPONENT_PARSER_HPP

#include <vector>
#include <string>
#include <map>

#include <ginac/ginac.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/vector.hpp>

#include "../symbol.hpp"
#include "../component.hpp"

namespace qi = boost::spirit::qi;

struct component_proxy
{
    component_types type;
    std::string name;
    std::vector<std::string> nodes;
    GiNaC::symbol value;
};

BOOST_FUSION_ADAPT_STRUCT(
    component_proxy,
    (component_types, type)
    (std::string, name)
    (std::vector<std::string>, nodes)
    (GiNaC::symbol, value)
)

/*
 * TODO: this is needed if numeric values should be supported (they should!)
 * However, for this the components need to store a GiNaC::ex, not a GiNaC::symbol as value
static GiNaC::ex check_and_convert_numeric_symbol(const std::string& v)
{
    GiNaC::ex value;
    if(v.size() > 0 && v.find_first_not_of("0123456789.-") == std::string::npos) // is the string a numeric?
    {
        value = std::stod(v);
    }
    else
    {
        value = get_symbol(v);
    }
    return value;
}
*/

namespace boost { namespace spirit { namespace traits
{
    template <>
    struct transform_attribute<component_types, char, qi::domain>
    {
        typedef char type;
        static char pre(__attribute__((__unused__)) component_types& c) { return ' '; } //not useful in this case but required to avoid compiler errors
        static void post(component_types& val, const char& attr) //`val` is the "returned" string, `attr` is what int_ parses
        {
            std::map<char, component_types> attribute_map {
                { 'R', ct_resistor                           },
                { 'C', ct_capacitor                          },
                { 'L', ct_inductor                           },
                { 'V', ct_voltage_source                     },
                { 'I', ct_current_source                     },
                { 'O', ct_opamp                              },
                { 'E', ct_voltage_controlled_voltage_source  },
                { 'F', ct_current_controlled_voltage_source  },
                { 'G', ct_voltage_controlled_current_source  },
                { 'H', ct_current_controlled_current_source  }
            };
            val = attribute_map[attr];
        }
        static void fail(component_types&) {}
    };

    template <>
    struct transform_attribute<GiNaC::symbol, std::string, qi::domain>
    {
        typedef std::string type;
        static std::string pre(__attribute__((__unused__)) GiNaC::symbol& c) { return ""; } //not useful in this case but required to avoid compiler errors
        static void post(GiNaC::symbol& val, const std::string& attr) //`val` is the "returned" string, `attr` is what int_ parses
        {
            val = get_symbol(attr);
        }
        static void fail(GiNaC::symbol&) {}
    };
}}}

struct component_parser_type : public qi::grammar<std::string::iterator, qi::ascii::blank_type, component_proxy()>
{
    typedef std::string::iterator Iterator;

    component_parser_type() : component_parser_type::base_type(main, "component")
    {
        using qi::attr;
        using qi::attr_cast;
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

        two_terminal_device   = attr_cast(char_("RCLVI")) >> name >> terminals(2) >> attr_cast(value);
        three_terminal_device = attr_cast(char_("O"))     >> name >> terminals(3) >> attr("OPDUMMY");
        four_terminal_device  = attr_cast(char_("EFGH"))  >> name >> terminals(4) >> attr_cast(value);

        main = two_terminal_device   |
               three_terminal_device |
               four_terminal_device;
    }

    qi::rule<Iterator, std::string()> name, terminal, value;
    qi::rule<Iterator, qi::ascii::blank_type, std::vector<std::string>(int)> terminals;
    qi::rule<Iterator, qi::ascii::blank_type, component_proxy()> two_terminal_device, three_terminal_device, four_terminal_device;
    qi::rule<Iterator, qi::ascii::blank_type, component_proxy()> main;
} component_parser;

#endif // COMPONENT_PARSER_HPP
