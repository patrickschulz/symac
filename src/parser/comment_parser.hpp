#ifndef COMMENT_PARSER_HPP
#define COMMENT_PARSER_HPP

#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

struct comment_parser_type : public qi::grammar<std::string::iterator, qi::ascii::blank_type, std::string()>
{
    typedef std::string::iterator Iterator;

    comment_parser_type() : comment_parser_type::base_type(main, "comment")
    {
        using qi::char_;
        using qi::ascii::space;
        using qi::eol;

        main = "*" >> *(char_ - eol);
    }

    qi::rule<Iterator, qi::ascii::blank_type, std::string()> main;

} comment_parser;

#endif // COMMENT_PARSER_HPP
