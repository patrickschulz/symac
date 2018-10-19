#ifndef COMPONENT_PARSER_HPP
#define COMPONENT_PARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/adapt_adt.hpp>
#include <boost/phoenix/phoenix.hpp>

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

struct component_type_type : qi::symbols<char, component_types>
{
    component_type_type()
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
} component_type;

class component
{
    public:
        friend std::ostream& operator<<(std::ostream& stream, const component& c);

        void set_type(component_types ct)
        {
            type = ct;
        }

        void set_nodes(const std::vector<std::string>& n)
        {
            nodes = n;
        }

        void set_value(const std::string& v)
        {
            value = v;
        }

        component_types get_type() const
        {
            return type;
        }

        const std::vector<std::string>& get_nodes() const
        {
            return nodes;
        }

        const std::string& get_value() const
        {
            return value;
        }

    private:
        component_types type;
        std::vector<std::string> nodes;
        std::string value;
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

BOOST_FUSION_ADAPT_ADT(
    component,
    (component_types, component_types, obj.get_type(), obj.set_type(val))
    (const std::vector<std::string>&, const std::vector<std::string>&, obj.get_nodes(), obj.set_nodes(val))
    (std::string, const std::string&, obj.get_value(), obj.set_value(val))
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

struct component_parser_type : public qi::grammar<std::string::iterator, SKIPPER, component(), qi::locals<component_types>>
{
    typedef std::string::iterator Iterator;

    component_parser_type() : component_parser_type::base_type(main, "component")
    {
        using qi::alnum;
        using qi::char_;
        using qi::repeat;
        using qi::ascii::space;
        using qi::_1;
        using qi::_a;
        using qi::_r1;
        using boost::phoenix::bind;

        type      %= component_type;
        terminal  %= +(alnum | char_("-:_!"));
        terminals %= repeat(bind(number, _r1))[terminal];
        value     %= +(char_ - space);
        main      %= type[_a = _1] >> terminals(_a) >> value;
    }

    qi::rule<Iterator, component_types()> type;
    qi::rule<Iterator, std::string()> terminal;
    qi::rule<Iterator, std::string()> value;
    qi::rule<Iterator, SKIPPER, std::vector<std::string>(component_types)> terminals;
    qi::rule<Iterator, SKIPPER, component(), qi::locals<component_types>> main;
} component_parser;

/*
template<typename Skipper>
struct component_parser_type : public qi::grammar<std::string::iterator, Skipper, component(), qi::locals<component_types>>
{
    typedef std::string::iterator Iterator;

    component_parser_type() : component_parser_type::base_type(main)
    {
        using qi::alnum;
        using qi::char_;
        using qi::repeat;
        using qi::ascii::space;
        using qi::_1;
        using qi::_a;
        using qi::_r1;
        using boost::phoenix::bind;

        type      %= component_type;
        terminal  %= +(alnum | char_("-:_!"));
        terminals %= repeat(bind(number, _r1))[terminal];
        value     %= +(char_ - space);
        main      %= type[_a = _1] >> terminals(_a) >> value;
    }

    qi::rule<Iterator, component_types()> type;
    qi::rule<Iterator, std::string()> terminal;
    qi::rule<Iterator, std::string()> value;
    qi::rule<Iterator, Skipper, std::vector<std::string>(component_types)> terminals;
    qi::rule<Iterator, Skipper, component(), qi::locals<component_types>> main;
};
*/

#endif // COMPONENT_PARSER_HPP
