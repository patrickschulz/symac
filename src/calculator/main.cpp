#include <boost/spirit/include/qi.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/foreach.hpp>

#include <ginac/ginac.h>

#include <iostream>
#include <string>
#include <map>

namespace qi = boost::spirit::qi;

const GiNaC::symbol& get_symbol(const std::string& str)
{
    static std::map<std::string, GiNaC::symbol> directory;
    auto it = directory.find(str);
    if (it != directory.end())
    {
        return it->second;
    }
    else
    {
        auto ret = directory.insert(std::make_pair(str, GiNaC::symbol(str)));
        return ret.first->second;
    }
}

namespace ast
{
    struct nil {};
    struct signed_;
    struct program;

    typedef boost::variant<
            nil
          , std::string
          , boost::recursive_wrapper<signed_>
          , boost::recursive_wrapper<program>
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

    struct program
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
    ast::program,
    (ast::operand, first)
    (std::list<ast::operation>, rest)
)

namespace ast
{
    struct eval
    {
        typedef GiNaC::ex result_type;

        result_type operator()(nil) const { BOOST_ASSERT(0); return 0; }

        result_type operator()(const std::string& s) const { return get_symbol(s); }

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

        result_type operator()(program const& x) const
        {
            result_type state = boost::apply_visitor(*this, x.first);
            BOOST_FOREACH(operation const& oper, x.rest)
            {
                state = (*this)(oper, state);
            }
            return state;
        }
    };
}

template <typename Iterator>
struct symbolic_calculator : qi::grammar<Iterator, ast::program(), qi::blank_type>
{
    symbolic_calculator() : symbolic_calculator::base_type(expression)
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

    qi::rule<Iterator, ast::program(), qi::blank_type> expression;
    qi::rule<Iterator, ast::program(), qi::blank_type> term;
    qi::rule<Iterator, ast::operand(), qi::blank_type> factor;
    qi::rule<Iterator, std::string()> identifier;
};

int main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Expression parser...\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Type an expression...or [q or Q] to quit\n\n";

    typedef std::string::const_iterator iterator_type;
    typedef ast::program ast_program;
    typedef ast::eval ast_eval;

    std::string str;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        symbolic_calculator<iterator_type> symcalc;
        ast_program program;
        ast_eval eval;

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        bool r = phrase_parse(iter, end, symcalc, qi::blank, program);

        if (r && iter == end)
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << "\nResult: " << eval(program) << std::endl;
            std::cout << "-------------------------\n";
        }
        else
        {
            std::string rest(iter, end);
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "stopped at: \" " << rest << "\"\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
