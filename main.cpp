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
            const std::string mode = commandline_options["mode"].as<std::string>();
            solver S(mode, nlist);
            S.mna();
            if(commandline_options.count("print"))
            {
                S.print_matrices();
            }
            S.solve();
            S.print();

            /*
            GiNaC::matrix res;
            if(!commandline_options.count("nosolve"))
            {
                res = A.solve(x, z, GiNaC::solve_algo::gauss);
                //std::vector<result> results;
                //results.push_back(result("v2"));
                //print_results(nlist, res, x, results);
                print_results(nlist, res);
                std::cout << '\n';
            }
            if(commandline_options.count("report"))
            {
                create_report(nlist, res);
            }
            */
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

