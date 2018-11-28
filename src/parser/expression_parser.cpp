#include "expression_parser.hpp"

symbolic_expression_type::symbolic_expression_type(const qi::rule<std::string::iterator, std::string()> idf) : 
    symbolic_expression_type::base_type(expression),
    identifier(idf)
{
    using qi::char_;
    using qi::alpha;
    using qi::alnum;
    using qi::lit;

    expression =
        term
        >> *(   (char_('+') >> term)
                |   (char_('-') >> term)
            )
        >> qi::eoi
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

}

// expression checking and evaluation
namespace ast
{
    checker::checker(const std::map<std::string, GiNaC::ex>& m) :
        symbolmap(m)
    { }

    bool checker::operator()(nil) const { BOOST_ASSERT(0); return 0; }

    bool checker::operator()(const std::string& s) const 
    {
        auto it = symbolmap.find(s);
        return it != symbolmap.end();
    }

    bool checker::operator()(operation const& x, bool lhs) const
    {
        bool rhs = boost::apply_visitor(*this, x.operand_);
        return lhs && rhs;
    }

    bool checker::operator()(signed_ const& x) const
    {
        bool rhs = boost::apply_visitor(*this, x.operand_);
        return rhs;
    }

    bool checker::operator()(expression const& x) const
    {
        bool state = boost::apply_visitor(*this, x.first);
        BOOST_FOREACH(operation const& oper, x.rest)
        {
            state = (*this)(oper, state);
        }
        return state;
    }

    /*
    eval::eval(const std::map<std::string, GiNaC::ex>& m) :
        symbolmap(m)
    { }
    */
    eval::eval(std::function<GiNaC::ex(const std::string&)> f) :
        symbolfunc(f)
    { }

    eval::result_type eval::operator()(nil) const { BOOST_ASSERT(0); return 0; }

    eval::result_type eval::operator()(const std::string& s) const 
    {
        return symbolfunc(s);
        //auto it = symbolmap.find(s);
        //return it->second;
    }

    eval::result_type eval::operator()(operation const& x, result_type lhs) const
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

    eval::result_type eval::operator()(signed_ const& x) const
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

    eval::result_type eval::operator()(expression const& x) const
    {
        result_type state = boost::apply_visitor(*this, x.first);
        BOOST_FOREACH(operation const& oper, x.rest)
        {
            state = (*this)(oper, state);
        }
        return state;
    }
}
