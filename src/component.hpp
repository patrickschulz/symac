#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>
#include <string>
#include <ginac/ginac.h>

enum component_types
{
    ct_none                              = 0,
    ct_resistor                          = 1 << 0,
    ct_capacitor                         = 1 << 1,
    ct_inductor                          = 1 << 2,
    ct_voltage_source                    = 1 << 3,
    ct_current_source                    = 1 << 4,
    ct_opamp                             = 1 << 5,
    ct_voltage_controlled_voltage_source = 1 << 6,
    ct_current_controlled_voltage_source = 1 << 7,
    ct_voltage_controlled_current_source = 1 << 8,
    ct_current_controlled_current_source = 1 << 9
};

struct component_proxy
{
    component_types type;
    std::string name;
    std::vector<std::string> nodes;
    std::string value;
};

class component
{
    public:
        component() = default;
        component(const component_proxy& p);
        component(const std::string& name, component_types type, const std::vector<std::string>& nodes, const GiNaC::symbol& value);
        
        std::string get_name() const;

        // results
        std::vector<std::string> get_terminal_names() const;

        void name_prepend(const std::string& prefix);

        unsigned int element_size() const;

        // getter and setter
        const component_types& get_type() const;

        const std::vector<std::string>& get_nodes() const;
        void set_nodes(const std::vector<std::string>& nodes);

        const GiNaC::symbol& get_value() const;

        std::string to_string() const;

        friend std::ostream& operator<<(std::ostream& stream, const component& c);

        bool operator==(component_types) const;

    private:
        std::string name;
        component_types type;
        std::vector<std::string> nodes;
        GiNaC::symbol value;

        // constant component attributes
        unsigned int mna_size;
        std::vector<std::string> terminals;
};

#endif //COMPONENT_HPP
