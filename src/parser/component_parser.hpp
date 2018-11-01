#ifndef COMPONENT_PARSER_HPP
#define COMPONENT_PARSER_HPP

#include <vector>
#include <string>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/vector.hpp>

namespace qi = boost::spirit::qi;

enum component_types
{
    ct_none                              = 0,
    ct_resistor                          = 1 << 0,
    ct_capacitor                         = 1 << 1,
    ct_inductor                          = 1 << 2,
    ct_voltage_source                    = 1 << 3,
    ct_current_source                    = 1 << 4,
    ct_opamp                             = 1 << 5,
    ct_voltage_controlled_voltage_source = 1 << 6,
    ct_current_controlled_voltage_source = 1 << 7,
    ct_voltage_controlled_current_source = 1 << 8,
    ct_current_controlled_current_source = 1 << 9
};

class component
{
    public:
        char type;
        std::vector<std::string> nodes;
        std::string value;

        friend std::ostream& operator<<(std::ostream& stream, const component& c);
};

std::ostream& operator<<(std::ostream& stream, const component& c)
{
    stream << c.type << " (" << c.value << ") { ";
    for(auto n : c.nodes)
    {
        stream << n << ' ';
    }
    stream << '}';
    return stream;
}

BOOST_FUSION_ADAPT_STRUCT(
    component,
    (char, type)
    (std::vector<std::string>, nodes)
    (std::string, value)
)

int number(char ct)
{
    std::map<char, unsigned int> number_of_terminals {
        { ct_resistor,                          2 },
        { ct_capacitor,                         2 },
        { ct_inductor,                          2 },
        { ct_voltage_source,                    2 },
        { ct_current_source,                    2 },
        { ct_opamp,                             3 },
        { ct_voltage_controlled_voltage_source, 4 },
        { ct_current_controlled_voltage_source, 4 },
        { ct_voltage_controlled_current_source, 4 },
        { ct_current_controlled_current_source, 4 }
    };
    return number_of_terminals[ct];
}

struct component_type_type : qi::symbols<char, char>
{
    component_type_type()
    {
        add
            ("R", 'R')
            ("C", 'C')
            ("L", 'L')
            ("V", 'V')
            ("I", 'I')
            ("O", 'O')
            ("E", 'E')
            ("F", 'F')
            ("G", 'G')
            ("H", 'H')
        ;
    }
} component_type;

struct component_parser_type : public qi::grammar<std::string::iterator, component()>
{
    typedef std::string::iterator Iterator;

    component_parser_type() : component_parser_type::base_type(main, "component")
    {
        using qi::omit;
        using qi::blank;
        using qi::alnum;
        using qi::char_;
        using qi::repeat;
        using qi::eol;

        terminal = +(alnum | char_("-:_!"));
        value    = +(char_ - eol);

        two_terminal_device   = char_("RCLVI") >> repeat(2)[omit[+blank] >> terminal] >> omit[+blank] >> value;
        three_terminal_device = char_("O")     >> repeat(3)[omit[+blank] >> terminal];
        four_terminal_device  = char_("EFGH")  >> repeat(4)[omit[+blank] >> terminal] >> omit[+blank] >> value;

        main = two_terminal_device   |
               three_terminal_device |
               four_terminal_device;
    }

    qi::rule<Iterator, std::string()> terminal, value;
    qi::rule<Iterator, component()> two_terminal_device, three_terminal_device, four_terminal_device;
    qi::rule<Iterator, component()> main;
} component_parser;

#endif // COMPONENT_PARSER_HPP
