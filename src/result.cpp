#include "result.hpp"

#include <iostream>
#include <map>
#include <boost/format.hpp>

#include "component.hpp"

#include "parser/expression_parser.hpp"

result::result(const componentlist& components, const GiNaC::matrix& results, const nodemap& nmap)
{
    // parse results and store all in the results map
    unsigned int row = 0;
    for(; row < components.number_of_nodes(); ++row) // node voltages
    {
        std::string usernode = nmap[row + 1];
        resultmap.insert(std::make_pair(usernode, results(row, 0)));
    }
    for(const component& c : components) // device currents
    {
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
        ast::expression expression;

        bool r = phrase_parse(cmd.content.begin(), cmd.content.end(), symbolic_expression, qi::blank, expression);
        if (r)
        {
            ast::checker checker(resultmap);
            if(checker(expression))
            {
                ast::eval eval(resultmap);
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

