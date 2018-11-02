#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <string>
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

struct command_parser_type : public qi::grammar<std::string::iterator, qi::ascii::blank_type, std::string()>
{
    typedef std::string::iterator Iterator;

    command_parser_type() : command_parser_type::base_type(main, "command")
    {
        using qi::char_;
        using qi::eol;

        command  = ".print";
        content = +(char_ - eol);
        main = command >> content;
    }

    qi::rule<Iterator> command;
    qi::rule<Iterator, std::string()> content;
    qi::rule<Iterator, qi::ascii::blank_type, std::string()> main;
} command_parser;

#endif // COMMAND_PARSER_HPP
