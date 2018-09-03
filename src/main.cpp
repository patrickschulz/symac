#include <iostream>
#include <vector>
#include <string>

#include "netlist.hpp"
#include "options.hpp"
#include "solver.hpp"
#include "result.hpp"

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
            std::vector<std::string> nodes = commandline_options["nodes"].as<std::vector<std::string>>();
            solver S(nlist.get_components());
            S.mna();
            if(commandline_options.count("print"))
            {
                S.print_matrices();
            }
            const std::string mode  = commandline_options["mode"].as<std::string>();
            result res = S.solve();
            res.print(mode);
            if(commandline_options.count("matlab_export"))
            {
                //res.export_matlab();
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

