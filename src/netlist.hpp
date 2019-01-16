#ifndef NETLIST_HPP
#define NETLIST_HPP

#include <vector>
#include <map>

#include "componentlist.hpp"
#include "command.hpp"
#include "simplification.hpp"

class netlist
{
    public:
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

        operator bool();

    private:
        componentlist components;
        std::vector<command> commands;
        std::vector<simplification> simplifications;

        bool valid;
};

#endif //NETLIST_HPP
