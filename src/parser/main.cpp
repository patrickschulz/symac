#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/phoenix/phoenix.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <map>

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

struct component_parser_ : qi::symbols<char, component_types>
{
    component_parser_()
    {
        add
            ("R", ct_resistor)
            ("C", ct_capacitor)
            ("L", ct_inductor)
            ("V", ct_voltage_source)
            ("I", ct_current_source)
            ("O", ct_opamp)
            ("E", ct_voltage_controlled_voltage_source)
            ("F", ct_current_controlled_voltage_source)
            ("G", ct_voltage_controlled_current_source)
            ("H", ct_current_controlled_current_source)
        ;
    }
} component_parser;

struct component
{
    component_types type;
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
    (component_types, type)
    (std::vector<std::string>, nodes)
    (std::string, value)
)


int number(component_types ct)
{
    std::map<component_types, unsigned int> number_of_terminals {
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

int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tParser Test\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Type a Test String ...or [q or Q] to quit\n\n";

    typedef std::string::const_iterator iterator_type;
    using qi::_a;
    using qi::_1;
    using boost::phoenix::bind;
    using qi::repeat;
    using qi::omit;
    using qi::space;
    using qi::alnum;
    using qi::char_;

    std::string str;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        qi::rule<std::string::iterator, component(), qi::locals<int>> component_line;
        component_line %= component_parser[_a = bind(number, _1)] >> repeat(_a)[omit[+space] >> +alnum] >> omit[+space] >> +char_;

        component result;
        auto iter = str.begin();
        bool r = qi::parse(iter, str.end(), component_line, result);

        if (r && iter == str.end())
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << result << '\n';
            std::cout << "-------------------------\n";
        }
        else
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
