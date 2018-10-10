#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/adapt_adt.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <boost/variant.hpp>

namespace qi = boost::spirit::qi;

struct command_parser_type : public qi::grammar<std::string::iterator, qi::ascii::space_type, std::string()>
{
    typedef std::string::iterator Iterator;

    command_parser_type() : command_parser_type::base_type(main)
    {
        using qi::char_;

        command  = ".print";
        body    %= +char_;
        main    %= command >> body;
    }

    qi::rule<Iterator> command;
    qi::rule<Iterator, std::string()> body;
    qi::rule<Iterator, qi::ascii::space_type, std::string()> main;
} command_parser;
