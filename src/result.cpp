#include "result.hpp"

#include <iostream>
#include <map>
#include <boost/format.hpp>

#include "component.hpp"

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

void result::print_voltage(const std::string& voltage) const
{
    auto it = resultmap.find(voltage);
    std::cout << "V(" << voltage << ")";
    if(it != resultmap.end())
    {
        std::cout << " = " << it->second << '\n';
    }
    else
    {
        std::cout << " not found\n";
    }
}

void result::print_voltage(const std::string& voltage1, const std::string& voltage2) const
{
    auto it1 = resultmap.find(voltage1);
    auto it2 = resultmap.find(voltage2);
    std::cout << voltage1 << " - " << voltage2 << " = " << it1->second - it2->second << '\n';
}

void result::print_current(const std::string& device) const
{
    auto it = resultmap.find(device);
    std::cout << "I(" << device << ")";
    if(it != resultmap.end())
    {
        std::cout << " = " << it->second << '\n';
    }
    else
    {
        std::cout << " not found\n";
    }
}

void result::print_helper(const std::string& p) const
{
    char domain = p[0];
    std::string symbol = p.substr(2, p.size() - 3);
    if(domain == 'V')
    {
        print_voltage(symbol);
    }
    else
    {
        print_current(symbol);
    }
}


void result::print(const std::vector<command>& print_cmd) const
{
    std::cout << GiNaC::csrc;
    for(const command& cmd : print_cmd)
    {
        if(cmd.content == "all")
        {
            print_all();
        }
        else
        {
            std::istringstream stream(cmd.content);
            std::vector<std::string> entries;
            std::string tmp;
            while(stream >> tmp)
            {
                entries.push_back(tmp);
            }
            if(entries.size() > 1)
            {
                std::string op1 = entries[0].substr(2, entries[0].size() - 3);
                std::string op2 = entries[2].substr(2, entries[2].size() - 3);
                auto it1 = resultmap.find(op1);
                auto it2 = resultmap.find(op2);
                GiNaC::ex res = it1->second / it2->second;
                res = res.expand();
                res = res.normal();
                std::cout << res << '\n';
            }
            else
            {
                print_helper(cmd.content);
            }
        }
    }
}

