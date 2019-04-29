#include "result.hpp"

#include <iostream>
#include <map>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include "component.hpp"
#include "parser/expression_parser.hpp"
#include "symbol.hpp"
#include "transfer_function/transfer_function.hpp"
#include "simplification/simplification.hpp"

BOOST_FUSION_ADAPT_STRUCT(
    quantity,
    (std::string, function)
    (std::string, symbol)
)

result::result()
{
    // insert ground
    add("V", "0", 0);
}

std::vector<std::string> chop_arguments(const quantity& q)
{
    std::vector<std::string> arguments;
    boost::char_separator<char> sep(",");
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    tokenizer tok(q.symbol, sep);
    std::copy(tok.begin(), tok.end(), std::back_inserter(arguments));
    // strip whitespace
    std::for_each(arguments.begin(), arguments.end(), std::bind(boost::trim<std::string>, std::placeholders::_1, std::locale()));
    return arguments;
}

std::string concat_arguments(const std::vector<std::string>& arguments, const std::string& sep)
{
    std::string res;
    for(unsigned int i = 0; i < arguments.size() - 1; ++i)
    {
        res += arguments[i];
        res += sep;
    }
    res += arguments[arguments.size() - 1];
    return res;
}

bool check_expression(const ast::expression<quantity>& expression, const resultmap_t& resultmap)
{
    auto f = [](const quantity& q, const resultmap_t& resmap) -> bool
    {
        if(q.function == "NUMBER")
        {
            return true;
        }
        std::string key = q.symbol;
        if(q.function == "Z" || q.function == "Y" || q.function == "S" || q.function == "NTF" || q.function == "H" || q.function == "G" || q.function == "ABCD")
        {
            std::vector<std::string> args = chop_arguments(q);
            key.clear();
            key = args[0] + "," + args[1];
        }
        return resmap.exists(q.function, key);
    };
    using namespace std::placeholders;
    ast::checker<quantity> checker(std::bind(f, _1, resultmap));
    return checker(expression);
}

GiNaC::ex evaluate_expression(const ast::expression<quantity>& expression, const resultmap_t& resultmap)
{
    auto f = [](const quantity& q, const resultmap_t& resmap) -> GiNaC::ex
    {
        if(q.function == "NUMBER")
        {
            return std::stod(q.symbol);
        }
        std::string key = q.symbol;
        if(q.function == "Z" || q.function == "Y" || q.function == "S" || q.function == "NTF" || q.function == "H" || q.function == "G" || q.function == "ABCD")
        {
            std::vector<std::string> args = chop_arguments(q);
            key.clear();
            key = args[0] + "," + args[1];
        }
        return resmap.get(q.function, key);
    };
    using namespace std::placeholders;
    ast::eval<quantity, GiNaC::ex> eval(std::bind(f, _1, resultmap));
    return eval(expression);
}

void print_command(command cmd, const symbolic_expression_type<quantity>& symbolic_expression, const resultmap_t& resultmap, const weightmap_t& weightmap, bool pretty, bool simpl)
{
    ast::expression<quantity> expression;

    bool r = phrase_parse(cmd.content.begin(), cmd.content.end(), symbolic_expression, qi::blank, expression);
    if (r)
    {
        if(check_expression(expression, resultmap))
        {
            GiNaC::ex res = evaluate_expression(expression, resultmap);
            transfer_function tf(res);
            if(simpl)
            {
                tf = simplify(tf, weightmap);
            }
            if(pretty)
            {
                tf.pretty_print(std::cout, cmd.content + " = ");
            }
            else
            {
                std::cout << tf.to_ginac(get_complex_symbol("s")) << '\n';
            }
        }
        else
        {
            std::cerr << "expression " << '"' <<  cmd.content << '"' << " contains unknown symbols\n";
        }
    }
    else
    {
        std::cerr << "could not parse expression " << '"' << cmd.content << '"' << '\n';
    }
}

void result::print(const std::vector<command>& print_cmd, bool pretty, bool simpl) const
{
    // create the expression parser
    qi::rule<Iterator, std::string()> function = +qi::alpha;
    qi::rule<Iterator, std::string()> symbol = +(qi::alnum | qi::char_(" ,."));
    qi::rule<Iterator, std::string()> number = +qi::digit >> -(qi::char_(".") >> +qi::digit);
    qi::rule<Iterator, quantity(), Skipper_type> identifier = (function >> "(" >> symbol >> ")") | (qi::attr("NUMBER") >> number);
    symbolic_expression_type<quantity> symbolic_expression(identifier);

    std::cout << GiNaC::dflt; // set output format
    for(const command& cmd : print_cmd)
    {
        print_command(cmd, symbolic_expression, resultmap, weightmap, pretty, simpl);
    }
}

void result::set_weightmap(const weightmap_t& wm)
{
    weightmap = wm;
}
