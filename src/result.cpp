#include "result.hpp"

#include <iostream>
#include <map>
#include <boost/format.hpp>

#include "component.hpp"

result::result(const componentlist& components,  const GiNaC::matrix& results, const nodemap& nmap) :
    components(components), results(results), nmap(nmap)
{

}

void result::print_voltage(const std::string& voltage) const
{
    unsigned int inode = nmap[voltage];
    std::cout << results(inode - 1, 0) << '\n';
}

void result::print_voltage(const std::string& voltage1, const std::string& voltage2) const
{
    unsigned int inode1 = nmap[voltage1];
    unsigned int inode2 = nmap[voltage2];
    std::cout << results(inode1 - 1, 0) - results(inode2 - 1, 0) << '\n';
}

void result::print(const std::string& mode) const
{
    if(mode == "ac")
    {
        std::cout << "Results:\n";
        std::cout << GiNaC::csrc;
        unsigned int row = 0;

        std::string indentation = std::string(8, ' ');
        
        std::cout << "    Node voltages:\n";
        for(; row < components.number_of_nodes(); ++row)
        {
            std::string usernode = nmap[row + 1];
            boost::format fmter = boost::format("%s%-20s") % (indentation) % ("Node " + usernode + ": ");
            std::string str = fmter.str();
            std::cout << str << results(row, 0) << '\n';
        }
        std::cout << "    Device Currents:\n";
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
        for(const component& c : components)
        {
            auto& display = results_display_map[c.get_type()];
            const auto& currents = display.first;
            ++display.second;
            for(const std::string& current : currents)
            {
                boost::format fmter = boost::format("%s%-20s") % (indentation) % ("Current " + current + std::to_string(display.second) + ": ");
                std::string str = fmter.str();
                std::cout << str << results(row, 0) << '\n';
                ++row;
            }
        }
    }
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
        /*
        if(matlab_export)
        {
            vvtf_matlab_export(filename,first,second);
        }
        if(nlist.is_simplification())
        { 
            std::string H_simple = vvtf_simplification(H);
            std::cout << " Simplified (in Latex) "<< '\n';
            std::cout << "H(s) = " << H_simple << '\n';
        }
        */
    }
    else
    {
        std::cerr << "unknown mode '" << mode << "' given\n";
    }
}

