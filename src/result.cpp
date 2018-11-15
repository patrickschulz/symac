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

void result::print_all() const
{
    for(auto entry : resultmap)
    {
        std::cout << entry.first << " = " << entry.second << '\n';
    }
}

void result::print(const std::vector<command>& print_cmd) const
{
    std::cout << GiNaC::csrc;
    for(command cmd : print_cmd)
    {
        ast::expression expression;
        ast::eval eval(resultmap);

        phrase_parse(cmd.content.begin(), cmd.content.end(), symbolic_expression, qi::blank, expression);
        std::cout << eval(expression) << '\n';

        /*
        if (r)
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            std::cout << "\nResult: " << eval(expression) << std::endl;
            std::cout << "-------------------------\n";
        }
        else
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
        */
    }
}

