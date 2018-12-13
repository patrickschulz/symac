#include "result.hpp"

#include <iostream>
#include <map>
#include <boost/format.hpp>

#include "component.hpp"
#include "parser/expression_parser.hpp"
#include "symbol.hpp"

struct get_quantity_
{
    get_quantity_(const std::map<std::string, GiNaC::ex>& m) :
        resultmap(m)
    { }

    GiNaC::ex operator()(const std::string& s)
    {
        auto it = resultmap.find(s);
        return it->second;
    }

    const std::map<std::string, GiNaC::ex> resultmap;
};

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
            case ct_inductor:
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

void result::print(const std::vector<command>& print_cmd) const
{
    std::cout << GiNaC::csrc;
    for(command cmd : print_cmd)
    {
        // create the expression parser
        qi::rule<std::string::iterator, std::string()> voltage = "V(" >> +(qi::alnum | qi::char_("-:_!")) >> ")";
        qi::rule<std::string::iterator, std::string()> current = "I(" >> +qi::alnum >> qi::char_(".") >> +qi::alpha >> ")";
        qi::rule<std::string::iterator, std::string()> identifier = voltage | current;
        symbolic_expression_type<std::string> symbolic_expression(identifier);

        ast::expression<std::string> expression;

        bool r = phrase_parse(cmd.content.begin(), cmd.content.end(), symbolic_expression, qi::blank, expression);
        if (r)
        {
            auto f = [](const std::string& s, const std::map<std::string, GiNaC::ex>& resmap) -> bool
            {
                auto it = resmap.find(s);
                return it != resmap.end();
            };
            using namespace std::placeholders;
            ast::checker<std::string> checker(std::bind(f, _1, resultmap));
            if(checker(expression))
            {
                get_quantity_ get_quantity(resultmap);
                ast::eval<std::string, GiNaC::ex> eval(get_quantity);
                std::cout << eval(expression) << '\n';
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
}

