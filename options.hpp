#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <cstdlib>
//#include "program_options/program_options.hpp"
#include <boost/program_options.hpp>

void print_usage(const boost::program_options::options_description& opt)
{
    std::cout << "Usage: symac [OPTION] netlist\n"
              << "simulate netlist\n"
              << opt
              << '\n';
}

void print_information()
{
    std::cout << "Structure of the network matrices:\n\n"
              << "The network satisfies the equation\n"
              << "    z = A * x\n"
              << "where\n"
              << "    z: excitations (independent voltage and current sources)\n"
              << "    A: network matrices containing the stamps of all components\n"
              << "    x: the variables of the network which are to be determined\n"
              << '\n'
              << "from top to bottom the x vector contains the following entries:\n"
              << " - node voltages\n"
              << " - currents through passives (resistors, capacitors and inductors)\n"
              << " - current through voltage sources\n"
              << " - output currents of opamps\n"
              << " - current through voltage controlled voltage sources\n"
              << " - input current of current controlled voltage sources\n"
              << " - output current of current controlled voltage sources\n"
              << " - input current of current controlled current sources"
              << '\n';
}

void print_devices()
{
    std::cout << "Available devices:\n"
              << "Two-terminal devices:\n"
              << "  Voltage sources: V N1 N2 VALUE\n"
              << "  Current sources: I N1 N2 VALUE\n"
              << "  Resistors:       R N1 N2 VALUE\n"
              << "  Capacitors:      C N1 N2 VALUE\n"
              << "  Inductors:       L N1 N2 VALUE\n"
              << "Three-terminal devices:\n"
              << "  Operational amplifiers: O IN1 IN2 OUT VALUE\n"
              << "Four-terminal devices (Cx are the controlling terminals (plus and minus)):\n"
              << "  Voltage-controlled voltage sources: E N1 N2 C1 C2 VALUE\n"
              << "  Current-controlled voltage sources: F N1 N2 C1 C2 VALUE\n"
              << "  Voltage-controlled current sources: G N1 N2 C1 C2 VALUE\n"
              << "  Current-controlled current sources: H N1 N2 C1 C2 VALUE\n"
              ;
}

void print_version()
{
    std::cout << "Symac 0.1\n";
}

class options
{
    public:
        options(int argc, char** argv)
        {
            using namespace boost::program_options;
            options_description generic("Generic options");
            generic.add_options()
                ("help,h"    , "produce help message")
                ("version,v" , "show version")
                ("print,p"   , "print network matrices")
                ("info,i"    , "print information about the network matrices")
                ("devices,D" , "print information about the usage of all components")
                ("nosolve,s" , "don't solve the network")
                ("report,r"  , "create a report")
                ("mode,m", value<std::string>()->default_value("ac"), "simulation mode (possible values: ac (default), sp (S-Parameters), tf (transfer function)")
                ("node,n", value<std::vector<std::string>>(), "nodes for transfer function (pass two times)")
                ("result"    , "which results should be shown? (show all if none are given)")
                ;

            options_description hidden("Hidden options");
            hidden.add_options()
                ("netlist", value<std::string>(), "netlist for simulation")
                ;

            // put together all options
            options_description all("Allowed options");
            all.add(generic);
            all.add(hidden);

            options_description visible("");
            visible.add(generic);

            positional_options_description p;
            p.add("netlist", -1);

            // parse arguments
            try
            {
                // set up parser
                command_line_parser parser(argc, argv);

                // set style
                using namespace boost::program_options::command_line_style;
                parser.style(allow_short          | // short options (-d)
                        short_allow_adjacent | // parameter for short option in same token (-d2)
                        short_allow_next     | // parameter for short option in next token (-d 2)
                        allow_long           | // long options (--foo)
                        long_allow_adjacent  | // parameter for long option in same token (-foo=2)
                        long_allow_next      | // parameter for long option in next token (-foo 2)
                        allow_sticky         | // group short options without parameter together (-df is the same as -d -f)
                        allow_dash_for_short   // use a dash (-) for short options
                        );

                // add options
                parser.options(all);
                parser.positional(p);

                // run the parser and store options
                store(parser.run(), commandline_options);
            }
            catch(boost::program_options::error& err) // error during parsing of command line
            {
                //error(err.what(), FATAL);
            }

            // update bound values
            notify(commandline_options);

            if(commandline_options.count("help"))
            {
                print_usage(visible);
                exit(0);
            }
            if(commandline_options.count("info"))
            {
                print_information();
                exit(0);
            }
            if(commandline_options.count("devices"))
            {
                print_devices();
                exit(0);
            }
            if(commandline_options.count("version"))
            {
                print_version();
                exit(0);
            }
        }

        const boost::program_options::variables_map& get_options() { return commandline_options; }
    private:
        boost::program_options::variables_map commandline_options;
};

#endif // OPTIONS_HPP
