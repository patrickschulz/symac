#include <boost/spirit/include/qi.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <string>

namespace qi = boost::spirit::qi;

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
    struct printer
    {
        typedef void result_type;

        void operator()(nil) const { std::cout << "nil"; }
        void operator()(const std::string& s) const { std::cout << s; }

        void operator()(operation const& x) const
        {
            boost::apply_visitor(*this, x.operand_);
            switch (x.operator_)
            {
                case '+': std::cout << " add"; break;
                case '-': std::cout << " sub"; break;
                case '*': std::cout << " mult"; break;
                case '/': std::cout << " div"; break;
            }
        }

        void operator()(signed_ const& x) const
        {
            boost::apply_visitor(*this, x.operand_);
            switch (x.sign)
            {
                case '-': std::cout << " neg"; break;
                case '+': std::cout << " pos"; break;
            }
        }

        void operator()(program const& x) const
        {
            boost::apply_visitor(*this, x.first);
            BOOST_FOREACH(operation const& oper, x.rest)
            {
                std::cout << ' ';
                (*this)(oper);
            }
        }
    };

    struct eval
    {
        typedef double result_type;

        double operator()(nil) const { BOOST_ASSERT(0); return 0; }
        double operator()(double n) const { return n; }

        double operator()(const std::string& s) const { return 1.0; }

        double operator()(operation const& x, double lhs) const
        {
            double rhs = boost::apply_visitor(*this, x.operand_);
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

        double operator()(signed_ const& x) const
        {
            double rhs = boost::apply_visitor(*this, x.operand_);
            switch (x.sign)
            {
                case '-': return -rhs;
                case '+': return +rhs;
            }
            BOOST_ASSERT(0);
            return 0;
        }

        double operator()(program const& x) const
        {
            double state = boost::apply_visitor(*this, x.first);
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

        identifier = alpha >> *alnum;
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
    typedef ast::printer ast_print;
    typedef ast::eval ast_eval;

    std::string str;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        symbolic_calculator<iterator_type> symcalc;
        ast_program program;
        ast_print print;
        ast_eval eval;

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        bool r = phrase_parse(iter, end, symcalc, qi::blank);

        if (r && iter == end)
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            print(program);
            //std::cout << "\nResult: " << eval(program) << std::endl;
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
