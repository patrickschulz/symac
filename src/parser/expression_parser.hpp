#ifndef EXPRESSION_PARSER_HPP
#define EXPRESSION_PARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/foreach.hpp>

#include <ginac/ginac.h>

#include <iostream>
#include <string>
#include <map>

#include "../symbol.hpp"

namespace qi = boost::spirit::qi;

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
        checker(const std::map<std::string, GiNaC::ex>& m);

        bool operator()(nil) const;
        bool operator()(const std::string& s) const;
        bool operator()(operation const& x, bool lhs) const;
        bool operator()(signed_ const& x) const;
        bool operator()(expression const& x) const;

        const std::map<std::string, GiNaC::ex>& resultmap;
    };

    struct eval
    {
        typedef GiNaC::ex result_type;

        eval(const std::map<std::string, GiNaC::ex>& m);
        result_type operator()(nil) const;
        result_type operator()(const std::string& s) const;
        result_type operator()(operation const& x, result_type lhs) const;
        result_type operator()(signed_ const& x) const;
        result_type operator()(expression const& x) const;

        const std::map<std::string, GiNaC::ex>& resultmap;
    };
}

struct symbolic_expression_type : qi::grammar<std::string::iterator, ast::expression(), qi::blank_type>
{
    symbolic_expression_type(const qi::rule<std::string::iterator, std::string()> idf);

    qi::rule<std::string::iterator, ast::expression(), qi::blank_type> expression, term;
    qi::rule<std::string::iterator, ast::operand(), qi::blank_type> factor;
    qi::rule<std::string::iterator, std::string()> identifier;
};

extern symbolic_expression_type symbolic_expression;

#endif // EXPRESSION_PARSER_HPP
