#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>

#include "netlist.hpp"
#include "mna.hpp"
#include "stamp.hpp"
#include "util.hpp"
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
            bool to_matlab = false;
            const std::string mode  = commandline_options["mode"].as<std::string>();
            std::vector<std::string> nodes;
            if(commandline_options.count("node1") && commandline_options.count("node2"))
            {
                std::string node1= commandline_options["node1"].as<std::string>();
                std::string node2= commandline_options["node2"].as<std::string>();
                nodes.push_back(node1);
                nodes.push_back(node2);
            }
            if(commandline_options.count("matlab_export"))
            {
                to_matlab = true;
            }
            solver S(filename, mode, nodes, nlist, to_matlab);
            S.mna();
            if(commandline_options.count("print"))
            {
                S.print_matrices();
            }
            S.solve();
            S.print();
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

