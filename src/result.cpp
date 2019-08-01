#include "result.hpp"

#include <iostream>
#include <map>
#include <algorithm>
#include <fstream>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include "component.hpp"
#include "parser/expression_parser.hpp"
#include "symbol.hpp"
#include "transfer_function/transfer_function.hpp"
#include "simplification/simplification.hpp"
#include "export/html.hpp"

std::string make_key(const std::string& str1, const std::string& str2)
{
    return str1 + "." + str2;
}

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

        std::vector<std::string> args = chop_arguments(q);
        std::string key = concat_arguments(args, ",");

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

        std::vector<std::string> args = chop_arguments(q);
        std::string key = concat_arguments(args, ",");

        return resmap.get(q.function, key);
    };
    using namespace std::placeholders;
    ast::eval<quantity, GiNaC::ex> eval(std::bind(f, _1, resultmap));
    return eval(expression);
}

std::string print_command(command cmd, const symbolic_expression_type<quantity>& symbolic_expression, const resultmap_t& resultmap, const weightmap_t& weightmap, bool pretty, bool simpl)
{
    ast::expression<quantity> expression;

    std::ostringstream stream;
    stream << GiNaC::dflt; // set output format

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
                tf.pretty_print(stream, cmd.content + " = ");
            }
            else
            {
                stream << tf.to_ginac(get_complex_symbol("s"));
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
    return stream.str();
}

std::string write_html_command(command cmd, const symbolic_expression_type<quantity>& symbolic_expression, const resultmap_t& resultmap, const weightmap_t& weightmap, bool simpl)
{
    ast::expression<quantity> expression;

    std::ostringstream stream;
    stream << GiNaC::latex; // set output format
    stream << "<p>" << '\n';
    stream << cmd.content << " = \\(";

    bool r = phrase_parse(cmd.content.begin(), cmd.content.end(), symbolic_expression, qi::blank, expression);
    if (r)
    {
        if(check_expression(expression, resultmap))
        {
            GiNaC::ex res = evaluate_expression(expression, resultmap);
            transfer_function tf(res);
            stream << tf.to_ginac(get_complex_symbol("s"));
        }
    }
    stream << "\\)" << '\n';
    stream << "</p>" << '\n';
    return stream.str();
}

void result::print_all() const
{
    for(auto e : resultmap)
    {
        std::cout << e.first << " = " << e.second << '\n';
    }
}

void result::report(const std::vector<command>& print_cmd, bool pretty, bool simpl) const
{
    std::ofstream html_file;

    if(html)
    {
        html_file.open("report.html");
        write_html_header(html_file);
    }

    // create the expression parser
    qi::rule<Iterator, std::string()> function = +qi::alpha;
    qi::rule<Iterator, std::string()> symbol = +(qi::alnum | qi::char_(" ,."));
    qi::rule<Iterator, std::string()> number = +qi::digit >> -(qi::char_(".") >> +qi::digit);
    qi::rule<Iterator, quantity(), Skipper_type> identifier = (function >> "(" >> symbol >> ")") | (qi::attr("NUMBER") >> number);
    symbolic_expression_type<quantity> symbolic_expression(identifier);

    for(const command& cmd : print_cmd)
    {
        std::cout << print_command(cmd, symbolic_expression, resultmap, weightmap, pretty, simpl) << '\n';
        if(html)
        {
            html_file << write_html_command(cmd, symbolic_expression, resultmap, weightmap, simpl) << '\n';
        }
    }

    if(html)
    {
        write_html_footer(html_file);
    }
}

void result::set_weightmap(const weightmap_t& wm)
{
    weightmap = wm;
}
