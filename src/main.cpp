#include <iostream>
#include <string>

#include "netlist.hpp"
#include "options.hpp"
#include "solver/solver.hpp"
#include "result.hpp"

int main(int argc, char** argv)
{
    options commandline_options(argc, argv);
    auto opt = commandline_options.get_options();
    if(opt.count("netlist"))
    {
        std::string filename = opt["netlist"].as<std::string>();
        netlist nlist;
        nlist.read(filename);
        if(nlist)
        {
            if(!opt.count("nosolve"))
            {
                result res;
                res.enable_html_report(opt.count("html"));
                auto weightmap = compute_weightmap(nlist.get_inequalities());
                res.set_weightmap(weightmap);

                solver S(nlist.get_components(), res);
                S.solve(opt.count("linearize"), opt.count("print"));

                res.report(nlist.get_print_cmds(), opt.count("pretty"), opt.count("simplify"));
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

