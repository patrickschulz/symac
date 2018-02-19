#ifndef NETLIST_HPP
#define NETLIST_HPP

#include <vector>
#include <iterator>
#include <memory>
#include <map>

#include "component.hpp"

class netlist
{
    public:
        netlist();
        netlist(const std::string& filename);

        void add_component(std::unique_ptr<component>&&);

        void read(std::string filename);
        void print_all_components() const;
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

    private:
        void reset();
        void update();

        bool valid;
        std::vector<std::unique_ptr<component>> components;

        std::map<std::string, unsigned int> nodemap;

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
};

#endif //NETLIST_HPP
