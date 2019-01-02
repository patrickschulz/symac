#ifndef EXPRESSION_PARSER_HPP
#define EXPRESSION_PARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/foreach.hpp>

#include <ginac/ginac.h>

#include "parser_common.hpp"

namespace ast
{
    struct nil {};
    template<typename atom_type> struct signed_;
    template<typename atom_type> struct expression;

    template<typename atom_type>
    using operand = typename boost::variant
        <
            nil,
            atom_type,
            boost::recursive_wrapper<signed_<atom_type>>,
            boost::recursive_wrapper<expression<atom_type>>
        >;

    template<typename atom_type>
    struct signed_
    {
        char sign;
        operand<atom_type> operand_;
    };

    template<typename atom_type>
    struct operation
    {
        char operator_;
        operand<atom_type> operand_;
    };

    template<typename atom_type>
    struct expression
    {
        operand<atom_type> first;
        std::list<operation<atom_type>> rest;
    };
}

BOOST_FUSION_ADAPT_TPL_STRUCT(
    (atom_type),
    (ast::signed_) (atom_type),
    (char, sign)
    (typename ast::operand<atom_type>, operand_)
)
BOOST_FUSION_ADAPT_TPL_STRUCT(
    (atom_type),
    (ast::operation) (atom_type),
    (char, operator_)
    (typename ast::operand<atom_type>, operand_)
)

BOOST_FUSION_ADAPT_TPL_STRUCT(
    (atom_type),
    (ast::expression) (atom_type),
    (typename ast::operand<atom_type>, first)
    (typename std::list<ast::operation<atom_type>>, rest)
)

namespace ast
{
    template<typename atom_type>
    struct checker
    {
        checker(std::function<bool(const atom_type&)> f) :
            checkfun(f)
        { }

        bool operator()(nil) const { BOOST_ASSERT(0); return 0; }

        bool operator()(const atom_type& s) const 
        {
            return checkfun(s);
        }

        bool operator()(operation<atom_type> const& x, bool lhs) const
        {
            bool rhs = boost::apply_visitor(*this, x.operand_);
            return lhs && rhs;
        }

        bool operator()(signed_<atom_type> const& x) const
        {
            bool rhs = boost::apply_visitor(*this, x.operand_);
            return rhs;
        }

        bool operator()(expression<atom_type> const& x) const
        {
            bool state = boost::apply_visitor(*this, x.first);
            BOOST_FOREACH(operation<atom_type> const& oper, x.rest)
            {
                state = (*this)(oper, state);
            }
            return state;
        }

        const std::function<bool(const atom_type&)> checkfun;
    };

    template<typename atom_type, typename result_type>
    struct eval
    {
        eval(std::function<result_type(const atom_type&)> f) :
            symbolfunc(f)
        { }

        result_type operator()(nil) const { BOOST_ASSERT(0); return 0; }

        result_type operator()(const atom_type& s) const 
        {
            return symbolfunc(s);
        }

        result_type operator()(operation<atom_type> const& x, result_type lhs) const
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

        result_type operator()(signed_<atom_type> const& x) const
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

        result_type operator()(expression<atom_type> const& x) const
        {
            result_type state = boost::apply_visitor(*this, x.first);
            BOOST_FOREACH(operation<atom_type> const& oper, x.rest)
            {
                state = (*this)(oper, state);
            }
            return state;
        }

            const std::function<result_type(const atom_type&)> symbolfunc;
        };
}

template<typename atom_type>
struct symbolic_expression_type : qi::grammar<Iterator, ast::expression<atom_type>(), Skipper_type>
{
    symbolic_expression_type(const qi::rule<Iterator, atom_type(), Skipper_type> idf) : 
        symbolic_expression_type::base_type(expression),
        identifier(idf)
    {
        using qi::char_;

        expression =
            term
            >> *(   
                    (char_('+') >> term) |
                    (char_('-') >> term)
                )
            >> qi::eoi
            ;

        term =
            factor
            >> *(   
                    (char_('*') >> factor) |
                    (char_('/') >> factor)
                )
            ;

        factor =
            identifier
            |   '(' >> expression >> ')'
            |   (char_('-') >> factor)
            |   (char_('+') >> factor)
            ;
    }

    qi::rule<Iterator, ast::expression<atom_type>(), Skipper_type> expression, term;
    qi::rule<Iterator, ast::operand<atom_type>(), Skipper_type> factor;
    qi::rule<Iterator, atom_type(), Skipper_type> identifier;
};

#endif // EXPRESSION_PARSER_HPP
