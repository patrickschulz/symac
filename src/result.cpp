#include "result.hpp"

#include <iostream>
#include <map>
#include <boost/format.hpp>

#include "component.hpp"
#include "parser/expression_parser.hpp"

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
    // parse results and store all in the results map
    unsigned int row = 0;
    for(; row < components.number_of_nodes(); ++row) // node voltages
    {
        std::string usernode = nmap[row + 1];
        resultmap.insert(std::make_pair(usernode, results(row, 0)));
    }
    for(const component& c : components) // device currents
    {
        if(c.get_type() == ct_resistor)
        {
            GiNaC::ex value = c.get_value();
            std::vector<std::string> nodes = c.get_nodes();
            GiNaC::ex voltage = resultmap[nodes[0]] - resultmap[nodes[1]];
            resultmap.insert(std::make_pair(c.get_name() + ".p",  voltage / value));
            resultmap.insert(std::make_pair(c.get_name() + ".n", -voltage / value));
        }
        std::string current = c.get_name();
        auto terminals = c.get_terminal_names();
        for(const auto& terminal : terminals)
        {
            resultmap.insert(std::make_pair(current + terminal, results(row, 0)));
            ++row;
        }
    }
    // TODO: fill the result map with the remaining terminal currents
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
        symbolic_expression_type symbolic_expression(identifier);

        ast::expression expression;

        bool r = phrase_parse(cmd.content.begin(), cmd.content.end(), symbolic_expression, qi::blank, expression);
        if (r)
        {
            auto f = [](const std::string& s, const std::map<std::string, GiNaC::ex>& resmap)
            {
                auto it = resmap.find(s);
                return it != resmap.end();
            };
            using namespace std::placeholders;
            ast::checker checker(std::bind(f, _1, resultmap));
            if(checker(expression))
            {
                get_quantity_ get_quantity(resultmap);
                ast::eval eval(get_quantity);
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

