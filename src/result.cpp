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


void result::print(const std::vector<std::string>& print_cmd) const
{
    std::cout << GiNaC::csrc;
    for(const auto& cmd : print_cmd)
    {
        if(cmd == "all")
        {
            print_all();
        }
        else
        {
            std::istringstream stream(cmd);
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
                print_helper(cmd);
            }
        }
    }
    /*
    else if(mode == "tf")
    {
        std::vector<std::string> nodes { "1", "4" };
        if(nodes.size() != 2){
            std::cerr << "You have to specify two nodes (by using the -n argument two times)"<< '\n';
            exit(1);
        }
        if(nodes[0] == "0" || nodes[1]== "0" || nodes[0] == "GND" || nodes[1] == "GND" )
        {
            std::cerr << " You can't use the GND node (node 0)" << '\n';
            exit(1);
        }
        unsigned int first = nmap[nodes[0]];
        unsigned int second= nmap[nodes[1]];
        GiNaC::ex H = results(second - 1, 0) / results(first - 1, 0);
        std::cout << "H(s) = " << H.normal() << '\n';
        //std::cout << "H(s) to Latex" << '\n';
        //std::cout << "H(s) = " <<GiNaC::latex << H << '\n';
        //if(matlab_export)
        //{
        //    vvtf_matlab_export(filename,first,second);
        //}
        //if(nlist.is_simplification())
        //{ 
        //    std::string H_simple = vvtf_simplification(H);
        //    std::cout << " Simplified (in Latex) "<< '\n';
        //    std::cout << "H(s) = " << H_simple << '\n';
        //}
    }
    else
    {
        std::cerr << "unknown mode '" << mode << "' given\n";
    }
    */
}

