#ifndef COMMENT_PARSER_HPP
#define COMMENT_PARSER_HPP

#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

struct comment
{
    std::string content;
};

BOOST_FUSION_ADAPT_STRUCT(
    comment,
    (std::string, content)
)

struct comment_parser_type : public qi::grammar<std::string::iterator, qi::ascii::blank_type, comment()>
{
    typedef std::string::iterator Iterator;

    comment_parser_type() : comment_parser_type::base_type(main, "comment")
    {
        using qi::char_;
        using qi::eol;

        content = *(char_ - eol);
        main = "*" >> content;
    }

    qi::rule<Iterator, std::string()> content;
    qi::rule<Iterator, qi::ascii::blank_type, comment()> main;

} comment_parser;

#endif // COMMENT_PARSER_HPP
