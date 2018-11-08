#ifndef SUBCIRCUIT_PARSER_HPP
#define SUBCIRCUIT_PARSER_HPP

#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/vector.hpp>

#include "component_parser.hpp"

namespace qi = boost::spirit::qi;
namespace fu = boost::fusion;

struct subcircuit_proxy
{
    std::string name;
    std::vector<std::string> terminals;
    std::vector<component> components;
};

BOOST_FUSION_ADAPT_STRUCT(
    subcircuit_proxy,
    (std::string, name)
    (std::vector<std::string>, terminals)
    (std::vector<component>, components)
)

struct subcircuit_parser_type : public qi::grammar<std::string::iterator, qi::blank_type, subcircuit_proxy()>
{
    typedef std::string::iterator Iterator;

    subcircuit_parser_type() : subcircuit_parser_type::base_type(main, "subcircuit")
    {
        using qi::alnum;
        using qi::alpha;
        using qi::eol;

        name      = alpha >> *alnum;
        terminal  = alpha >> *alnum;
        terminals = +terminal;
        body      = component_parser % eol;

        main      = ".subckt" >> name >> terminals >> eol >> body >> eol >> ".end";
    }

    qi::rule<Iterator, std::string()> name, terminal;
    qi::rule<Iterator, qi::blank_type, std::vector<std::string>()> terminals;
    qi::rule<Iterator, qi::blank_type, std::vector<component>()> body;
    qi::rule<Iterator, qi::blank_type, subcircuit_proxy()> main;
} subcircuit_parser;

struct subcircuit_instance_proxy
{
    std::string name;
    std::vector<std::string> terminals;
};

BOOST_FUSION_ADAPT_STRUCT(
    subcircuit_instance_proxy,
    (std::string, name)
    (std::vector<std::string>, terminals)
)

struct subcircuit_instance_parser_type : public qi::grammar<std::string::iterator, qi::blank_type, subcircuit_instance_proxy()>
{
    typedef std::string::iterator Iterator;

    subcircuit_instance_parser_type() : subcircuit_instance_parser_type::base_type(main, "subcircuit_instance")
    {
        using qi::alnum;
        using qi::alpha;
        using qi::char_;
        using qi::repeat;
        using qi::_r1;

        identifier = char_("X");
        name = alpha >> * alnum;
        terminal = +(alnum | char_("-:_!"));
        //terminals = repeat(_r1)[terminal];
        terminals = +terminal;

        main = identifier >> name >> terminals(2);
    }

    qi::rule<Iterator> identifier;
    qi::rule<Iterator, std::string()> name, terminal;
    qi::rule<Iterator, qi::blank_type, std::vector<std::string>(int)> terminals;
    qi::rule<Iterator, qi::blank_type, subcircuit_instance_proxy()> main;
} subcircuit_instance_parser;

#endif // SUBCIRCUIT_PARSER_HPP
