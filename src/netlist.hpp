#ifndef NETLIST_HPP
#define NETLIST_HPP

#include <vector>
#include <map>

#include "componentlist.hpp"
#include "subcircuit.hpp"

class netlist
{
    public:
        void read(const std::string& filename);
        
        component component_read_in(const std::string& line);
        void add_component(const component& c);

        const componentlist& get_components() const
        {
            return components;
        }

        std::vector<std::string> get_print_cmds() const
        {
            return std::vector<std::string>{ "all" };
        }

        operator bool();

        void translate_subcircuit(const std::string& line);

        std::vector<std::string> get_values();

    private:
        componentlist components;

        bool valid;

        // subcircuits
        std::map<std::string, subcircuit> subcircuits;
        subcircuit* current_subcircuit;
};

#endif //NETLIST_HPP
