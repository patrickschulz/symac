#include "comment_parser.hpp"

comment_parser_type::comment_parser_type() : comment_parser_type::base_type(main, "comment")
{
    using qi::char_;
    using qi::eol;

    content = *(char_ - eol);
    main = "*" >> content;
}

comment_parser_type comment_parser;
