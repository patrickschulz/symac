#ifndef SUBCIRCUIT_PARSER_HPP
#define SUBCIRCUIT_PARSER_HPP

#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/vector.hpp>

#include "../subcircuit.hpp"
#include "component_parser.hpp"
#include "comment_parser.hpp"

#include "parser_common.hpp"

namespace qi = boost::spirit::qi;

BOOST_FUSION_ADAPT_STRUCT(
    subcircuit,
    (std::string, name)
    (std::vector<std::string>, terminals)
    (std::vector<component>, components)
)

struct subcircuit_parser_type : public qi::grammar<Iterator, Skipper_type, subcircuit()>
{
    subcircuit_parser_type() : subcircuit_parser_type::base_type(main, "subcircuit")
    {
        using qi::alnum;
        using qi::alpha;
        using qi::eol;
        using qi::omit;

        name      = alpha >> *alnum;
        terminal  = alpha >> *alnum;
        terminals = +terminal;
        body      = (component_parser | omit[comment_parser]) % eol;

        main      = ".subckt" >> name >> terminals >> eol >> body >> eol >> ".end";
    }

    qi::rule<Iterator, std::string()> name, terminal;
    qi::rule<Iterator, Skipper_type, std::vector<component>()> body;
    qi::rule<Iterator, Skipper_type, std::vector<std::string>()> terminals;
    qi::rule<Iterator, Skipper_type, subcircuit()> main;

    component_parser_type component_parser;
    comment_parser_type comment_parser;
};

BOOST_FUSION_ADAPT_STRUCT(
    subcircuit_instance,
    (std::string, instance)
    (std::string, name)
    (std::vector<std::string>, terminals)
)

struct subcircuit_instance_parser_type : public qi::grammar<Iterator, Skipper_type, subcircuit_instance()>
{
    subcircuit_instance_parser_type() : subcircuit_instance_parser_type::base_type(main, "subcircuit_instance")
    {
        using qi::alnum;
        using qi::alpha;
        using qi::char_;
        using qi::lit;

        identifier = lit("X") >> +alnum;
        name = alpha >> *alnum;
        terminal = +(alnum | char_("-:_!"));
        terminals = +terminal;

        main = identifier >> name >> terminals;
    }

    qi::rule<Iterator, std::string()> identifier;
    qi::rule<Iterator, std::string()> name, terminal;
    qi::rule<Iterator, Skipper_type, std::vector<std::string>()> terminals;
    qi::rule<Iterator, Skipper_type, subcircuit_instance()> main;
};

#endif // SUBCIRCUIT_PARSER_HPP
