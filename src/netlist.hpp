#ifndef NETLIST_HPP
#define NETLIST_HPP

#include <vector>
#include <map>

#include "componentlist.hpp"
#include "command.hpp"
#include "simplification/simplification.hpp"

// TODO: temporarily
#include "symbol.hpp"

class netlist
{
    public:
        netlist(bool linearize = false);

        void read(const std::string& filename);
        
        component component_read_in(const std::string& line);
        void add_component(const component& c);
        void add_command(const command& c);

        componentlist& get_components() 
        {
            return components;
        }

        std::vector<command> get_print_cmds() const
        {
            return commands;
        }

        std::vector<simplification> get_simplifications() const
        {
            return simplifications;
        }

        std::vector<inequality> get_inequalities() const
        {
            GiNaC::possymbol R1 = get_symbol("R1");
            GiNaC::possymbol R2 = get_symbol("R2");
            std::vector<inequality> inequalities {
                { R1, R2, ">>" }
            };
            return inequalities;
        }

        bool does_linearize() const
        {
            return linearize;
        }

        operator bool();

    private:
        componentlist components;
        std::vector<command> commands;
        std::vector<simplification> simplifications;
        std::vector<inequality> inequalities;

        bool linearize;

        bool valid;
};

#endif //NETLIST_HPP
