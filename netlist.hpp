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
//         std::string to_string();
        void read(std::string filename);
        void print_all_components() const;
        unsigned int numbr_terminals(char type);
        unsigned int number_of_nodes() const;
        const std::vector<const component*> get_devices(component_types type) const;
        unsigned int number_of_devices(component_types type) const;
        unsigned int number_of_impedances() const;
        int number_of_voltage_sources() const;
        
        unsigned int full_network_size() const;

        unsigned int get_current_impedance() { return current_impedance; }
        unsigned int get_current_voltage_source() { return current_source; }
        unsigned int get_current_opamp() { return current_opamp; }
        unsigned int get_current_vcvs() { return current_vcvs; }
        unsigned int get_current_ccvs() { return current_ccvs; }
        unsigned int get_current_cccs() { return current_cccs; }

        void increment_current_impedance() { ++current_impedance; }
        void increment_current_voltage_source() { ++current_source; }
        void increment_current_opamp() { ++current_opamp; }
        void increment_current_vcvs() { ++current_vcvs; }
        void increment_current_ccvs() { ++current_ccvs; }
        void increment_current_cccs() { ++current_cccs; }

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
        std::string change_subline_nodes(std::string line, std::vector<std::string> terminals, std::string subckt_name);
        std::string change_subline_terminals(std::string sline, std::vector<std::string> sub_t_names, std::vector<std::string> t_names);
        //matlab export
        void set_matlab_values(std::string v);
        std::vector<std::string> get_values();
        //command - simplification
        bool is_simplification();
        void set_simplification(const std::string& line);
        std::vector<std::string> get_simplifications();
        
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
        
        // network state
        unsigned int numnodes;
        unsigned int numsources;
        unsigned int numimpedances;
        unsigned int numopamps;
        unsigned int numvcvs;
        unsigned int numccvs;
        unsigned int numcccs;

        unsigned int current_impedance;
        unsigned int current_source;
        unsigned int current_opamp;
        unsigned int current_vcvs;
        unsigned int current_ccvs;
        unsigned int current_cccs;
        
        //command-simplification
        bool simplification ;
        std::vector<std::string> vals_simplify; 
};

#endif //NETLIST_HPP
