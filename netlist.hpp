#ifndef NETLIST_HPP
#define NETLIST_HPP

#include <vector>
#include <iterator>
#include <memory>
#include <map>

#include "component.hpp"
#include "nodemap.hpp"
#include "subcircuit.hpp"

class netlist
{
    public:
        netlist();
        netlist(const std::string& filename);

        void read(std::string filename);
        
        void component_read_in(const std::string& line);
        void add_component(const component& c);

        unsigned int number_terminals(char type);
        unsigned int number_of_nodes() const;
        const std::vector<component> get_devices(component_types type) const;
        unsigned int number_of_devices(component_types type) const;
        
        unsigned int full_network_size() const;

        const std::vector<component>& get_components() const
        {
            return components;
        }

        operator bool();

        std::string get_output_node(unsigned int node) const;
        unsigned int get_unode(const std::string& snode) const;

        // subcircuit
        bool is_subckt_call(const std::string& line);
        void subckt_call(const std::string& line);
        void read_subckt_line(const std::string& line, unsigned int number_subckt);
        void read_subckt_title(std::string& title);
        std::string change_subline_nodes(std::string line, std::vector<std::string> terminals, std::string subckt_name);
        std::string change_subline_terminals(std::string sline, std::vector<std::string> sub_t_names, std::vector<std::string> t_names);
        //matlab export
        void set_matlab_values(std::string v);
        std::vector<std::string> get_values();
        //command - simplification
        bool is_simplification();
        void set_simplification();
        std::map<std::string, unsigned int> get_simplifications();
        void save_simpl_line(const std::string& line);
        void change_simpl_map(std::string, std::string);
        //simplification level
        void set_simpl_level(const std::string&);
        std::string get_simpl_level();
        
    private:
        std::vector<component> components;

        bool valid;
        nodemap nmap;
        std::map<unsigned int, std::string> output_map;
        
        //subcircuit
        std::vector<subcircuit> subckt_vector;
        unsigned int number_subckt;

        //matlab output
        std::vector<std::string> values;
        
        //command-simplification
        bool simplification ;
        std::vector<std::string> simplify_lines; 
        std::map<std::string, unsigned int> simpl_map;
        //simplification level (english, medium, well-done)
        std::string simpl_level;
};

#endif //NETLIST_HPP
