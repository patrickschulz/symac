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
        netlist nlist;
        nlist.read(filename);
        if(nlist)
        {
            solver S(nlist.get_components());
            S.mna();
            if(commandline_options.count("print"))
            {
                S.print_matrices();
            }
            result res = S.solve();
            res.print(nlist.get_print_cmds());
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

