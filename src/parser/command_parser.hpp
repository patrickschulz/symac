#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/vector.hpp>

#include "../command.hpp"

#include "parser_common.hpp"

namespace qi = boost::spirit::qi;

BOOST_FUSION_ADAPT_STRUCT(
    command,
    (command_type, type)
    (std::string, content)
)

struct command_parser_type : public qi::grammar<Iterator, Skipper_type, command()>
{
    command_parser_type();

    qi::rule<Iterator, command_type()> identifier;
    qi::rule<Iterator, std::string()> content;
    qi::rule<Iterator, Skipper_type, command()> main;
};

#endif // COMMAND_PARSER_HPP
