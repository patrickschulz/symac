#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>
#include <ginac/ginac.h>

enum component_types
{
    ct_none,
    ct_resistor,
    ct_capacitor,
    ct_inductor,
    ct_voltage_source,
    ct_current_source,
    ct_opamp,
    ct_voltage_controlled_voltage_source,
    ct_current_controlled_voltage_source,
    ct_voltage_controlled_current_source,
    ct_current_controlled_current_source
};

class component
{
    public:
        component(const std::string& name, char type, const std::vector<std::string>& nodes, const GiNaC::ex& value);
        
        component_types get_type() const;
        std::string get_name() const;

        // results
        std::vector<std::string> get_terminal_names() const;

        void name_prepend(const std::string& prefix);

        unsigned int element_size() const;

        const std::vector<std::string>& get_nodes() const;
        void set_nodes(const std::vector<std::string>& nodes);

        GiNaC::ex get_value() const;

    private:
        std::string name;
        component_types type;
        std::vector<std::string> nodes;
        GiNaC::ex value;

        // constant component attributes
        unsigned int mna_size;
        std::vector<std::string> terminals;
};

#endif //COMPONENT_HPP
