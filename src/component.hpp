#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>
#include <string>
#include <ginac/ginac.h>

enum component_types
{
    ct_resistor,
    ct_conductor,
    ct_capacitor,
    ct_inductor,
    ct_voltage_source,
    ct_current_source,
    ct_opamp,
    ct_voltage_controlled_voltage_source,
    ct_current_controlled_voltage_source,
    ct_voltage_controlled_current_source,
    ct_current_controlled_current_source,
    ct_mosfet,
    ct_port,
};

struct parameter_t
{
    std::string key;
    std::string value;
};

struct spice_component_proxy
{
    component_types type;
    std::string name;
    std::vector<std::string> nodes;
    std::string value;
    std::vector<parameter_t> parameters;
};
struct spectre_component_proxy
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
        component(const component&) = default;

        component(const spice_component_proxy& p);
        component(const spectre_component_proxy& p);
        
        void set_name(const std::string&);
        std::string get_name() const;
        void name_prepend(const std::string& prefix);

        unsigned int element_size() const;

        bool is_noisy() const;
        GiNaC::ex get_noise() const;
        component get_noise_source() const;

        // getter and setter
        component_types get_type() const;
        void set_type(component_types ct);

        const std::vector<std::string>& get_nodes() const;
        void set_nodes(const std::vector<std::string>& nodes);

        const GiNaC::ex& get_value() const;
        void set_value(const GiNaC::ex& e);

        void set_parameter(const std::string& key, const std::string& value);
        std::string get_parameter(const std::string& key) const;

        friend std::ostream& operator<<(std::ostream& stream, const component& c);

        bool operator==(component_types) const;
        bool operator==(const component&) const;

    private:
        component(const std::string& name, component_types type, const std::vector<std::string>& nodes);

        std::string name;
        component_types type;
        std::vector<std::string> nodes;
        GiNaC::ex value;
        std::vector<parameter_t> parameters;

        // constant component attributes
        unsigned int mna_size;
};

// a small signal model can be made up of several components, therefore a vector of components is the return type
std::vector<component> get_small_signal_model(const component& c);

#endif //COMPONENT_HPP
