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

struct command_type_parser_type : qi::symbols<char, command_type>
{
    command_type_parser_type()
    {
        add
            (".print"  , com_print)
            (".printl" , com_printlatex)
            (".replace", com_replace)
        ;
    }
} command_type_parser;

struct command_parser_type : public qi::grammar<std::string::iterator, qi::blank_type, command()>
{
    typedef std::string::iterator Iterator;

    command_parser_type() : command_parser_type::base_type(main, "command")
    {
        using qi::char_;
        using qi::eol;
        using qi::lit;

        identifier = command_type_parser;
        content = +(char_ - eol);
        main = identifier >> content;
    }

    qi::rule<Iterator, command_type()> identifier;
    qi::rule<Iterator, std::string()> content;
    qi::rule<Iterator, qi::blank_type, command()> main;
} command_parser;

#endif // COMMAND_PARSER_HPP
