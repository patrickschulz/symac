#ifndef COMMENT_PARSER_HPP
#define COMMENT_PARSER_HPP

#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

struct comment_parser_type : public qi::grammar<std::string::iterator, SKIPPER, std::string()>
{
    typedef std::string::iterator Iterator;

    comment_parser_type() : comment_parser_type::base_type(main, "comment")
    {
        using qi::char_;
        using qi::ascii::space;
        using qi::eol;

        main = "*" >> *(char_ - eol);
    }

    qi::rule<Iterator, SKIPPER, std::string()> main;

} comment_parser;

/*
template<typename Skipper>
struct comment_parser_type : public qi::grammar<std::string::iterator, Skipper, std::string()>
{
    typedef std::string::iterator Iterator;

    comment_parser_type() : comment_parser_type::base_type(main)
    {
        using qi::char_;
        using qi::ascii::space;

        comment  = ".print";
        body    %= +(char_ - space);
        main    %= comment >> body;
    }

    qi::rule<Iterator> comment;
    qi::rule<Iterator, std::string()> body;
    qi::rule<Iterator, Skipper, std::string()> main;
};
*/

#endif // COMMENT_PARSER_HPP
