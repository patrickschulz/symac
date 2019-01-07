#include "result.hpp"

#include <iostream>
#include <map>
#include <boost/format.hpp>

#include "component.hpp"
#include "parser/expression_parser.hpp"
#include "symbol.hpp"
#include "transfer_function/transfer_function.hpp"

result::result(const componentlist& components, const GiNaC::matrix& results, const nodemap& nmap)
{
    // insert ground
    resultmap.insert(std::make_pair("0", GiNaC::ex(0)));

    // voltages
    for(unsigned int row = 0; row < components.number_of_nodes(); ++row)
    {
        std::string usernode = nmap[row + 1];
        resultmap.insert(std::make_pair(usernode, results(row, 0)));
    }

    // currents
    for(const component& c : components)
    {
        switch(c.get_type())
        {
            case ct_resistor:
            {
                GiNaC::ex value = c.get_value();
                std::vector<std::string> nodes = c.get_nodes();
                GiNaC::ex voltage = resultmap[nodes[0]] - resultmap[nodes[1]];
                boost::format fmter = boost::format("%s.%s");
                resultmap[str(fmter % c.get_name() % "p")] =  voltage / value;
                resultmap[str(fmter % c.get_name() % "n")] = -voltage / value;
                break;
            }
            case ct_conductor:
            {
                GiNaC::ex value = c.get_value();
                std::vector<std::string> nodes = c.get_nodes();
                GiNaC::ex voltage = resultmap[nodes[0]] - resultmap[nodes[1]];
                boost::format fmter = boost::format("%s.%s");
                resultmap[str(fmter % c.get_name() % "p")] =  voltage * value;
                resultmap[str(fmter % c.get_name() % "n")] = -voltage * value;
                break;
            }
            case ct_inductor:
            {
                GiNaC::ex value = c.get_value();
                std::vector<std::string> nodes = c.get_nodes();
                GiNaC::ex voltage = resultmap[nodes[0]] - resultmap[nodes[1]];
                boost::format fmter = boost::format("%s.%s");
                GiNaC::ex s = get_symbol("s");
                resultmap[str(fmter % c.get_name() % "p")] =  voltage / s / value;
                resultmap[str(fmter % c.get_name() % "n")] = -voltage / s / value;
                break;
            }
            case ct_capacitor:
            {
                GiNaC::ex value = c.get_value();
                std::vector<std::string> nodes = c.get_nodes();
                GiNaC::ex voltage = resultmap[nodes[0]] - resultmap[nodes[1]];
                boost::format fmter = boost::format("%s.%s");
                GiNaC::ex s = get_symbol("s");
                resultmap[str(fmter % c.get_name() % "p")] =  voltage * s * value;
                resultmap[str(fmter % c.get_name() % "n")] = -voltage * s * value;
                break;
            }
            case ct_voltage_controlled_current_source:
            {
                GiNaC::ex value = c.get_value();
                std::vector<std::string> nodes = c.get_nodes();
                GiNaC::ex voltage = resultmap[nodes[2]] - resultmap[nodes[3]];
                boost::format fmter = boost::format("%s.%s");
                resultmap[str(fmter % c.get_name() % "p")] =  voltage * value;
                resultmap[str(fmter % c.get_name() % "n")] = -voltage * value;
                break;
            }
            case ct_voltage_source:
            case ct_opamp:
            case ct_voltage_controlled_voltage_source:
            case ct_current_controlled_current_source:
            {
                unsigned int offset = components.number_of_nodes() + components.component_index(c);
                boost::format fmter = boost::format("%s.%s");
                resultmap[str(fmter % c.get_name() % "p")] =  results(offset, 0);
                resultmap[str(fmter % c.get_name() % "n")] = -results(offset, 0);
                break;
            }
            case ct_current_source:
            {
                boost::format fmter = boost::format("%s.%s");
                resultmap[str(fmter % c.get_name() % "p")] =  c.get_value();
                resultmap[str(fmter % c.get_name() % "n")] = -c.get_value();
                break;
            }
            case ct_current_controlled_voltage_source:
            {
                unsigned int offset = components.number_of_nodes() + components.component_index(c);
                boost::format fmter = boost::format("%s.%s");
                resultmap[str(fmter % c.get_name() % "cp")] =  results(offset, 0);
                resultmap[str(fmter % c.get_name() % "cn")] = -results(offset, 0);
                resultmap[str(fmter % c.get_name() % "p")] =  results(offset + 1, 0);
                resultmap[str(fmter % c.get_name() % "n")] = -results(offset + 1, 0);
                break;
            }
            case ct_port:
                // a port is an auxiliary device and thus has no currents
                break;
        }
    }
}

void result::add(const std::string& key, const GiNaC::ex& res)
{
    resultmap.insert(std::make_pair(key, res));
}

bool check_expression(const ast::expression<std::string>& expression, const std::map<std::string, GiNaC::ex>& resultmap)
{
    auto f = [](const std::string& s, const std::map<std::string, GiNaC::ex>& resmap) -> bool
    {
        auto it = resmap.find(s);
        return it != resmap.end();
    };
    using namespace std::placeholders;
    ast::checker<std::string> checker(std::bind(f, _1, resultmap));
    return checker(expression);
}

GiNaC::ex evaluate_expression(const ast::expression<std::string>& expression, const std::map<std::string, GiNaC::ex>& resultmap)
{
    auto f = [](const std::string& s, const std::map<std::string, GiNaC::ex>& resmap) -> GiNaC::ex
    {
        auto it = resmap.find(s);
        return it->second;
    };
    using namespace std::placeholders;
    ast::eval<std::string, GiNaC::ex> eval(std::bind(f, _1, resultmap));
    return eval(expression);
}

void print_command(command cmd, const symbolic_expression_type<std::string>& symbolic_expression, const std::map<std::string, GiNaC::ex>& resultmap, bool pretty)
{
    ast::expression<std::string> expression;

    bool r = phrase_parse(cmd.content.begin(), cmd.content.end(), symbolic_expression, qi::blank, expression);
    if (r)
    {
        if(check_expression(expression, resultmap))
        {
            GiNaC::ex res = evaluate_expression(expression, resultmap);
            transfer_function tf(res);
            if(pretty)
            {
                tf.pretty_print(std::cout, cmd.content + " = ");
            }
            else
            {
                std::cout << tf.to_ginac(get_symbol("s")) << '\n';
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

void result::print(const std::vector<command>& print_cmd, bool pretty) const
{
    // create the expression parser
    qi::rule<Iterator, std::string()> voltage = "V(" >> +(qi::alnum | qi::char_("-:_!")) >> ")";
    qi::rule<Iterator, std::string()> current = "I(" >> +qi::alnum >> qi::char_(".") >> +qi::alpha >> ")";
    qi::rule<Iterator, std::string(), Skipper_type> portval = qi::char_("ZYS") >> "(" >> +qi::digit >> qi::char_(",") >> +qi::digit >> ")";
    qi::rule<Iterator, std::string(), Skipper_type> identifier = voltage | current | portval;
    symbolic_expression_type<std::string> symbolic_expression(identifier);

    std::cout << GiNaC::dflt; // set output format
    for(const command& cmd : print_cmd)
    {
        print_command(cmd, symbolic_expression, resultmap, pretty);
    }
}

