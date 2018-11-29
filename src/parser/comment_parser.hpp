#ifndef COMMENT_PARSER_HPP
#define COMMENT_PARSER_HPP

#include "../comment.hpp"

#include <boost/spirit/include/qi.hpp>

#include "parser_common.hpp"

namespace qi = boost::spirit::qi;

BOOST_FUSION_ADAPT_STRUCT(
    comment,
    (std::string, content)
)

struct comment_parser_type : public qi::grammar<Iterator, comment()>
{
    comment_parser_type();

    qi::rule<Iterator, std::string()> content;
    qi::rule<Iterator, comment()> main;

};
extern comment_parser_type comment_parser;

#endif // COMMENT_PARSER_HPP
