#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <cstdlib>
#include <boost/program_options.hpp>

#include "solver.hpp"

void print_usage(const boost::program_options::options_description& opt)
{
    std::cout << "Usage: symac [OPTION] netlist\n"
              << "simulate netlist\n"
              << opt
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
    std::cout << "Symac 0.3\n";
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
                ("pretty,P"  , "print the results in a pretty way (display transfer functions as fractions) THIS IS PRELIMINARY, CHECK YOUR RESULTS!")
                ("reslist,N" , "print resulting netlist (after transformation of subcircuits)")
                ("devices,D" , "print information about the usage of all components")
                ;
            options_description unimplemented("Unimplemented options");
            unimplemented.add_options()
                ("nodemap"   , "print the nodemap (mapping from user (string-) nodes to internal (integer-) nodes)")
                ("nosolve,s" , "don't solve the network")
                ("report,r"  , "create a report")
                ("matlab"    , value<std::string>(), "export results to MatLab, specify filename")
                ;

            options_description hidden("Hidden options");
            hidden.add_options()
                ("netlist", value<std::string>(), "netlist for simulation")
                ;

            // put together all options
            options_description all("Allowed options");
            all.add(generic);
            all.add(unimplemented);
            all.add(hidden);

            options_description visible("");
            visible.add(generic);
            visible.add(unimplemented);

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
