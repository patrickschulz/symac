#include "result.hpp"

#include <iostream>
#include <map>
#include <boost/format.hpp>

#include "component.hpp"

result::result(const componentlist& components, const GiNaC::matrix& results, const nodemap& nmap)
{
    std::map<component_types, std::pair<std::vector<std::string>, unsigned int>> results_display_map { 
        { ct_resistor,                          { { "Ir"            }, 0 } }, 
        { ct_capacitor,                         { { "Ic"            }, 0 } }, 
        { ct_inductor,                          { { "Il"            }, 0 } }, 
        { ct_voltage_source,                    { { "Iv"            }, 0 } },
        { ct_opamp,                             { { "Iop"           }, 0 } },
        { ct_voltage_controlled_voltage_source, { { "I_vcvs"        }, 0 } },
        { ct_current_controlled_current_source, { { "I_cccs"        }, 0 } },
        { ct_current_controlled_voltage_source, { { "Ifin", "Ifout" }, 0 } },
        { ct_voltage_controlled_current_source, { {                 }, 0 } }, // dummy entry
        { ct_current_source                   , { {                 }, 0 } }  // dummy entry
    };

    // parse results and store all in the results map
    unsigned int row = 0;
    for(; row < components.number_of_nodes(); ++row) // node voltages
    {
        std::string usernode = nmap[row + 1];
        resultmap.insert(std::make_pair(usernode, results(row, 0)));
    }
    for(const component& c : components) // device currents
    {
        auto& display = results_display_map[c.get_type()];
        ++display.second;
        for(const std::string& current : display.first)
        {
            std::string key = current + std::to_string(display.second);
            resultmap.insert(std::make_pair(key, results(row, 0)));
            ++row;
        }
    }
}

void result::print_voltage(const std::string& voltage) const
{
    auto it = resultmap.find(voltage);
    std::cout << voltage << " = " << it->second << '\n';
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
    std::cout << device << " = " << it->second << '\n';
}

void result::print(const std::vector<std::string>& print_cmd) const
{
    std::cout << GiNaC::csrc;
    for(const auto& cmd : print_cmd)
    {
        char domain = cmd[0];
        std::string symbol = cmd.substr(2, cmd.size() - 3);
        if(domain == 'V')
        {
            print_voltage(symbol);
        }
        else
        {
            print_current(symbol);
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

