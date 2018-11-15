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
        checker(const std::map<std::string, GiNaC::ex>& m) :
            resultmap(m)
        { }

        bool operator()(nil) const { BOOST_ASSERT(0); return 0; }

        bool operator()(const std::string& s) const 
        {
            auto it = resultmap.find(s);
            return it != resultmap.end();
        }

        bool operator()(operation const& x, bool lhs) const
        {
            bool rhs = boost::apply_visitor(*this, x.operand_);
            return lhs && rhs;
        }

        bool operator()(signed_ const& x) const
        {
            bool rhs = boost::apply_visitor(*this, x.operand_);
            return rhs;
        }

        bool operator()(expression const& x) const
        {
            bool state = boost::apply_visitor(*this, x.first);
            BOOST_FOREACH(operation const& oper, x.rest)
            {
                state = (*this)(oper, state);
            }
            return state;
        }

        const std::map<std::string, GiNaC::ex>& resultmap;
    };

    struct eval
    {
        typedef GiNaC::ex result_type;

        eval(const std::map<std::string, GiNaC::ex>& m) :
            resultmap(m)
        { }

        result_type operator()(nil) const { BOOST_ASSERT(0); return 0; }

        result_type operator()(const std::string& s) const 
        {
            auto it = resultmap.find(s);
            return it->second;
        }

        result_type operator()(operation const& x, result_type lhs) const
        {
            result_type rhs = boost::apply_visitor(*this, x.operand_);
            switch (x.operator_)
            {
                case '+': return lhs + rhs;
                case '-': return lhs - rhs;
                case '*': return lhs * rhs;
                case '/': return lhs / rhs;
            }
            BOOST_ASSERT(0);
            return 0;
        }

        result_type operator()(signed_ const& x) const
        {
            result_type rhs = boost::apply_visitor(*this, x.operand_);
            switch (x.sign)
            {
                case '-': return -rhs;
                case '+': return +rhs;
            }
            BOOST_ASSERT(0);
            return 0;
        }

        result_type operator()(expression const& x) const
        {
            result_type state = boost::apply_visitor(*this, x.first);
            BOOST_FOREACH(operation const& oper, x.rest)
            {
                state = (*this)(oper, state);
            }
            return state;
        }

        const std::map<std::string, GiNaC::ex>& resultmap;
    };
}

struct symbolic_expression_type : qi::grammar<std::string::iterator, ast::expression(), qi::blank_type>
{
    symbolic_expression_type() : symbolic_expression_type::base_type(expression)
    {
        using qi::char_;
        using qi::alpha;
        using qi::alnum;

        expression =
            term
            >> *(   (char_('+') >> term)
                |   (char_('-') >> term)
                )
            ;

        term =
            factor
            >> *(   (char_('*') >> factor)
                |   (char_('/') >> factor)
                )
            ;

        factor =
                identifier
            |   '(' >> expression >> ')'
            |   (char_('-') >> factor)
            |   (char_('+') >> factor)
            ;

        identifier = "V(" >> alpha >> *alnum >> ")";
    }

    qi::rule<std::string::iterator, ast::expression(), qi::blank_type> expression;
    qi::rule<std::string::iterator, ast::expression(), qi::blank_type> term;
    qi::rule<std::string::iterator, ast::operand(), qi::blank_type> factor;
    qi::rule<std::string::iterator, std::string()> identifier;
} symbolic_expression;

#endif // EXPRESSION_PARSER_HPP
