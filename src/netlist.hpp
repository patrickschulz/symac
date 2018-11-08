#ifndef NETLIST_HPP
#define NETLIST_HPP

#include <vector>
#include <map>

#include "componentlist.hpp"
#include "command.hpp"

class netlist
{
    public:
        void read(const std::string& filename);
        
        component component_read_in(const std::string& line);
        void add_component(const component& c);
        void add_command(const command& c);

        const componentlist& get_components() const
        {
            return components;
        }

        std::vector<command> get_print_cmds() const
        {
            return commands;
        }

        operator bool();

    private:
        componentlist components;
        std::vector<command> commands;

        bool valid;
};

#endif //NETLIST_HPP
