#include <iostream>
#include <vector>
#include <string>

#include "netlist.hpp"
#include "options.hpp"
#include "solver.hpp"

int main(int argc, char** argv)
{
    options opt(argc, argv);
    auto commandline_options = opt.get_options();
    if(commandline_options.count("netlist"))
    {
        std::string filename = commandline_options["netlist"].as<std::string>();
        netlist nlist(filename);
        if(nlist)
        {
            const std::string mode  = commandline_options["mode"].as<std::string>();

            std::vector<std::string> nodes = commandline_options["nodes"].as<std::vector<std::string>>();
            solver S(mode, nlist.get_components());
            S.mna();
            if(commandline_options.count("print"))
            {
                S.print_matrices();
            }
            S.solve();
            S.print();
            if(commandline_options.count("matlab_export"))
            {
                //std::string matlab_export_mode = commandline_options["matlab_export"].as<std::string>();
                //S.export_to_matlab(matlab_export_mode);
            }
        }
        else
        {
            std::cout << "netlist is not valid\n";
        }
    }
    else
    {
        std::cout << "no netlist given\n";
        return 1;
    }
}

