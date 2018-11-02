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

class component
{
    public:
        component() = default;
        component(const std::string& name, char type, const std::vector<std::string>& nodes, const GiNaC::symbol& value);
        
        component_types get_type() const;
        std::string get_name() const;

        // results
        std::vector<std::string> get_terminal_names() const;

        void name_prepend(const std::string& prefix);

        unsigned int element_size() const;

        const std::vector<std::string>& get_nodes() const;
        void set_nodes(const std::vector<std::string>& nodes);

        std::string to_string() const;

        GiNaC::ex get_value() const;

    private:
        std::string name;
    public: // FIXME: hack for BOOST_ADAPT_STRUCT, since i currently i can't figure out how to use BOOST_ADAPT_ADT
        component_types type;
        std::vector<std::string> nodes;
        GiNaC::symbol value;

        friend std::ostream& operator<<(std::ostream& stream, const component& c);
    private:

        // constant component attributes
        unsigned int mna_size;
        std::vector<std::string> terminals;
};

#endif //COMPONENT_HPP
