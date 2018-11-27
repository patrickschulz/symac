#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/vector.hpp>

#include "../command.hpp"

namespace qi = boost::spirit::qi;

BOOST_FUSION_ADAPT_STRUCT(
    command,
    (command_type, type)
    (std::string, content)
)

struct command_parser_type : public qi::grammar<std::string::iterator, qi::blank_type, command()>
{
    command_parser_type();

    qi::rule<std::string::iterator, command_type()> identifier;
    qi::rule<std::string::iterator, std::string()> content;
    qi::rule<std::string::iterator, qi::blank_type, command()> main;
};

extern command_parser_type command_parser;

#endif // COMMAND_PARSER_HPP
