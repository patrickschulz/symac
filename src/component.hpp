#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>
#include <string>
#include <ginac/ginac.h>

#include "component_types.hpp"

struct parameter_t
{
    void set_key(const std::string& k) { key = k; }
    const std::string& get_key() const { return key; }
    void set_value(const std::string& v) { value = v; }
    const std::string& get_value() const { return value; }

    std::string key;
    std::string value;
};

struct spice_component_proxy
{
    public:
        void set_name(const std::string& n) { name = n; }
        const std::string& get_name() const { return name; }
        void set_type(component_types ct) { type = ct; }
        component_types get_type() const { return type; }
        void set_nodes(const std::vector<std::string>& n) { nodes = n; }
        const std::vector<std::string>& get_nodes() const { return nodes; }
        void set_value(const std::string& v) { value = v; }
        const std::string& get_value() const { return value; }
        void set_parameters(const std::vector<parameter_t>& p) { parameters = p; }
        const std::vector<parameter_t>& get_parameters() const { return parameters; }
    public:
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
        const std::string& get_name() const;
        void name_prepend(const std::string& prefix);

        bool is_noisy() const;
        component get_noise_source() const;

        // getter and setter
        component_types get_type() const;
        void set_type(component_types ct);

        const std::vector<std::string>& get_nodes() const;
        void set_nodes(const std::vector<std::string>& nodes);

        const GiNaC::ex& get_value() const;
        void set_value(const GiNaC::ex& e);

        void set_parameter(const std::string& key, const std::string& value);
        bool has_parameter(const std::string& key) const;
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
};

// a small signal model can be made up of several components, therefore a vector of components is the return type
std::vector<component> get_small_signal_model(const component& c);

#endif //COMPONENT_HPP
