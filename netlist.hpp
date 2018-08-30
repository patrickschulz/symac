#ifndef NETLIST_HPP
#define NETLIST_HPP

#include <vector>
#include <iterator>
#include <memory>
#include <map>

#include "component.hpp"
#include "map.hpp"
#include "subckt.hpp"

class netlist
{
    public:
        netlist();
        netlist(const std::string& filename);
        
        void component_read_in(const std::string& line);
        
        void add_component(std::unique_ptr<component>&&);
        void read(std::string filename);
        void print_all_components() const;
        unsigned int numbr_terminals(char type);
        unsigned int number_of_nodes() const;
        const std::vector<const component*> get_devices(component_types type) const;
        unsigned int number_of_devices(component_types type) const;
        unsigned int number_of_impedances() const;
        int number_of_voltage_sources() const;
        
        unsigned int full_network_size() const;

        const std::vector<std::unique_ptr<component>>& get_components() const
        {
            return components;
        }

        operator bool();
        // OUTPUT MAP
        void add_to_output_map(unsigned int node, std::string snode);
        std::string get_output_node(unsigned int node) const;
        unsigned int get_unode(std::string snode) const;
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
        void reset();
        void update();

        bool valid;
        std::vector<std::unique_ptr<component>> components;
        map nmap;
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
