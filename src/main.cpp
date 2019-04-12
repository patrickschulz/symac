#include <iostream>
#include <string>

#include "netlist.hpp"
#include "options.hpp"
#include "solver/solver.hpp"
#include "result.hpp"
#include "symbol.hpp"

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
            if(!commandline_options.count("nosolve"))
            {
                solver S(nlist.get_components());

                result res = S.solve
                    (
                        commandline_options.count("linearize"),
                        commandline_options.count("print")
                    );
                auto weightmap = compute_weightmap(nlist.get_inequalities());
                res.set_weightmap(weightmap);
                res.print
                    (
                        nlist.get_print_cmds(),
                        commandline_options.count("pretty"),
                        commandline_options.count("simplify")
                    );
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

