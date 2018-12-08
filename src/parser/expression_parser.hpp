#ifndef EXPRESSION_PARSER_HPP
#define EXPRESSION_PARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <ginac/ginac.h>

#include <string>

#include "../symbol.hpp"

#include "parser_common.hpp"

namespace ast
{
    struct nil {};
    struct signed_;
    struct expression;

    typedef boost::variant<
            nil
          , std::string
          , boost::recursive_wrapper<signed_>
          , boost::recursive_wrapper<expression>
        >
    operand;

    struct signed_
    {
        char sign;
        operand operand_;
    };

    struct operation
    {
        char operator_;
        operand operand_;
    };

    struct expression
    {
        operand first;
        std::list<operation> rest;
    };
}

BOOST_FUSION_ADAPT_STRUCT(
    ast::signed_,
    (char, sign)
    (ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::operation,
    (char, operator_)
    (ast::operand, operand_)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::expression,
    (ast::operand, first)
    (std::list<ast::operation>, rest)
)

namespace ast
{
    struct checker
    {
        checker(std::function<bool(const std::string&)> f);

        bool operator()(nil) const;
        bool operator()(const std::string& s) const;
        bool operator()(operation const& x, bool lhs) const;
        bool operator()(signed_ const& x) const;
        bool operator()(expression const& x) const;

        const std::function<bool(const std::string&)> symbolfunc;
    };

    struct eval
    {
        typedef GiNaC::ex result_type;

        eval(std::function<GiNaC::ex(const std::string&)> f);
        result_type operator()(nil) const;
        result_type operator()(const std::string& s) const;
        result_type operator()(operation const& x, result_type lhs) const;
        result_type operator()(signed_ const& x) const;
        result_type operator()(expression const& x) const;

        const std::function<GiNaC::ex(const std::string&)> symbolfunc;
    };
}

struct symbolic_expression_type : qi::grammar<Iterator, ast::expression(), Skipper_type>
{
    symbolic_expression_type(const qi::rule<Iterator, std::string()> idf);

    qi::rule<Iterator, ast::expression(), Skipper_type> expression, term;
    qi::rule<Iterator, ast::operand(), Skipper_type> factor;
    qi::rule<Iterator, std::string()> identifier;
};

#endif // EXPRESSION_PARSER_HPP
