#ifndef PARSER_COMMON_HPP
#define PARSER_COMMON_HPP

#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

typedef std::string::iterator Iterator;
typedef qi::blank_type Skipper_type;

#endif // PARSER_COMMON_HPP
