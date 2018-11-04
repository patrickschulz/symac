#ifndef SUBCIRCUIT_PARSER_HPP
#define SUBCIRCUIT_PARSER_HPP

#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/vector.hpp>

#include "component_parser.hpp"

namespace qi = boost::spirit::qi;
namespace fu = boost::fusion;

struct subcircuit_parser_type : public qi::grammar<std::string::iterator, qi::blank_type, fu::vector<std::string, std::vector<std::string>, std::vector<component>>()>
{
    typedef std::string::iterator Iterator;

    subcircuit_parser_type() : subcircuit_parser_type::base_type(main)
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
    qi::rule<Iterator, qi::blank_type, fu::vector<std::string, std::vector<std::string>, std::vector<component>>()> main;
} subcircuit_parser;

#endif // SUBCIRCUIT_PARSER_HPP
