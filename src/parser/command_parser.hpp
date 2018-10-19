#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

struct command_parser_type : public qi::grammar<std::string::iterator, SKIPPER, std::string()>
{
    typedef std::string::iterator Iterator;

    command_parser_type() : command_parser_type::base_type(main, "command")
    {
        using qi::char_;
        using qi::ascii::space;

        command  = ".print";
        body    %= +(char_ - space);
        main    %= command >> body;
    }

    qi::rule<Iterator> command;
    qi::rule<Iterator, std::string()> body;
    qi::rule<Iterator, SKIPPER, std::string()> main;
} command_parser;

/*
template<typename Skipper>
struct command_parser_type : public qi::grammar<std::string::iterator, Skipper, std::string()>
{
    typedef std::string::iterator Iterator;

    command_parser_type() : command_parser_type::base_type(main)
    {
        using qi::char_;
        using qi::ascii::space;

        command  = ".print";
        body    %= +(char_ - space);
        main    %= command >> body;
    }

    qi::rule<Iterator> command;
    qi::rule<Iterator, std::string()> body;
    qi::rule<Iterator, Skipper, std::string()> main;
};
*/

#endif // COMMAND_PARSER_HPP
