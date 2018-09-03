#include "result.hpp"

#include <map>
#include <boost/format.hpp>

#include "component.hpp"

result::result(const componentlist& components,  const GiNaC::matrix& results, const nodemap& nmap) :
    components(components), results(results), nmap(nmap)
{

}

void result::print(const std::string& mode) const
{
    if(mode == "ac")
    {
        std::cout << "Results:\n";
        std::cout << GiNaC::csrc;
        unsigned int row = 0;
        
        std::cout << "    Node voltages:\n";
        for(; row < components.number_of_nodes(); ++row)
        {
            std::string usernode = nmap[row + 1];
            boost::format fmter = boost::format("%sNode %s:\t\t") % ("        ") % (usernode);
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
            { ct_voltage_controlled_current_source, { {                 }, 0 } }
        };
        for(const component& c : components)
        {
            auto& display = results_display_map[c.get_type()];
            const auto& currents = display.first;
            ++display.second;
            for(const std::string& current : currents)
            {
                boost::format fmter = boost::format("%sCurrent %s%d:\t") % ("        ") % (current) % (display.second);
                std::string str = fmter.str();
                std::cout << str << results(row, 0) << '\n';
                ++row;
            }
        }
        /*
        for(const auto& dev : dev_formats)
        {
            std::string header;
            component_types ct;
            std::vector<std::string> currents;
            std::tie(header, ct, currents) = dev;
            unsigned int size = components.number_of_devices(ct);
            if(size > 0)
            {
                std::cout << "    " << header << ":\n";
                for(unsigned int i = 0; i < size; ++i)
                {
                    for(const auto& current : currents)
                    {
                        boost::format fmter = boost::format("        %s%d:\t\t") % current % (i + 1);
                        std::string str = fmter.str();
                        std::cout << str << results(row, 0) << '\n';
                        ++row;
                    }
                }
                std::cout << '\n';
            }
        }
        */
    }
    /*
    else if(mode == "tf")
    {
        if(nodes.size() != 2){
            std::cerr << "You have to specify two nodes (by using the -n argument two times)"<< '\n';
            exit(1);
        }
        if(nodes[0] == "0" || nodes[1]== "0" || nodes[0] == "GND" || nodes[1] == "GND" )
        {
            std::cerr << " You can't use the GND node (node 0)" << '\n';
            exit(1);
        }
        unsigned int first = nlist.get_unode(nodes[0]) - 1;
        unsigned int second= nlist.get_unode(nodes[1]) - 1;
        GiNaC::ex H = results(second, 0) / results(first, 0);
        std::cout << "H(s) = " << H << '\n';
        std::cout << "H(s) to Latex" << '\n';
        std::cout << "H(s) = " <<GiNaC::latex << H << '\n';
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
    }
    */
    else
    {
        std::cerr << "unknown mode '" << mode << "' given\n";
    }
}

