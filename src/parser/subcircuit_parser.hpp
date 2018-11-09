#ifndef SUBCIRCUIT_PARSER_HPP
#define SUBCIRCUIT_PARSER_HPP

#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/vector.hpp>

#include "component_parser.hpp"
#include "comment_parser.hpp"

namespace qi = boost::spirit::qi;
namespace fu = boost::fusion;

struct subcircuit
{
    std::string name;
    std::vector<std::string> terminals;
    std::vector<component> components;
};

BOOST_FUSION_ADAPT_STRUCT(
    subcircuit,
    (std::string, name)
    (std::vector<std::string>, terminals)
    (std::vector<component>, components)
)

struct subcircuit_parser_type : public qi::grammar<std::string::iterator, qi::blank_type, subcircuit()>
{
    typedef std::string::iterator Iterator;

    subcircuit_parser_type() : subcircuit_parser_type::base_type(main, "subcircuit")
    {
        using qi::alnum;
        using qi::alpha;
        using qi::eol;
        using qi::omit;

        name      = alpha >> *alnum;
        terminal  = alpha >> *alnum;
        terminals = +terminal;
        bodyline  = component_parser | omit[comment_parser];
        body      = bodyline % eol;

        main      = ".subckt" >> name >> terminals >> eol >> body >> eol >> ".end";
    }

    qi::rule<Iterator, std::string()> name, terminal;
    qi::rule<Iterator, qi::blank_type, component()> bodyline;
    qi::rule<Iterator, qi::blank_type, std::vector<component>()> body;
    qi::rule<Iterator, qi::blank_type, std::vector<std::string>()> terminals;
    qi::rule<Iterator, qi::blank_type, subcircuit()> main;
} subcircuit_parser;

struct subcircuit_instance
{
    std::string instance;
    std::string name;
    std::vector<std::string> terminals;
};

BOOST_FUSION_ADAPT_STRUCT(
    subcircuit_instance,
    (std::string, instance)
    (std::string, name)
    (std::vector<std::string>, terminals)
)

struct subcircuit_instance_parser_type : public qi::grammar<std::string::iterator, qi::blank_type, subcircuit_instance()>
{
    typedef std::string::iterator Iterator;

    subcircuit_instance_parser_type() : subcircuit_instance_parser_type::base_type(main, "subcircuit_instance")
    {
        using qi::alnum;
        using qi::alpha;
        using qi::char_;
        using qi::omit;

        identifier = omit[char_("X")] >> +alnum;
        name = alpha >> *alnum;
        terminal = +(alnum | char_("-:_!"));
        terminals = +terminal;

        main = identifier >> name >> terminals;
    }

    qi::rule<Iterator, std::string()> identifier;
    qi::rule<Iterator, std::string()> name, terminal;
    qi::rule<Iterator, qi::blank_type, std::vector<std::string>()> terminals;
    qi::rule<Iterator, qi::blank_type, subcircuit_instance()> main;
} subcircuit_instance_parser;

#endif // SUBCIRCUIT_PARSER_HPP
