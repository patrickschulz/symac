#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <vector>
#include <ginac/ginac.h>

#include "symbol.hpp"

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

inline component_types operator|(component_types a, component_types b)
{
    return static_cast<component_types>(static_cast<int>(a) | static_cast<int>(b));
}

const char component_names[] = {
    'R', // ct_resistor
    'C', // ct_capacitor
    'L', // ct_inductor
    'V', // ct_voltage_source
    'I', // ct_current_source
    'O', // ct_opamp
    'E', // ct_voltage_controlled_voltage_source
    'F', // ct_current_controlled_voltage_source
    'G', // ct_voltage_controlled_current_source
    'H', // ct_current_controlled_current_source
};

class component
{
    public:
        component(component_types type, const std::vector<unsigned int>& nodes, const GiNaC::ex& value) :
            type(type), nodes(nodes), value(value)
        {   }

        component(char type, const std::vector<unsigned int>& nodes, const GiNaC::ex& value) :
            nodes(nodes), value(value)
        {   
            switch(type)
            {
                case 'R':
                    this->type = ct_resistor;
                    break;
                case 'C':
                    this->type = ct_capacitor;
                    break;
                case 'L':
                    this->type = ct_inductor;
                    break;
                case 'V':
                    this->type = ct_voltage_source;
                    break;
                case 'I':
                    this->type = ct_current_source;
                    break;
                case 'O':
                    this->type = ct_opamp;
                    break;
                case 'E':
                    this->type = ct_voltage_controlled_voltage_source;
                    break;
                case 'F':
                    this->type = ct_current_controlled_voltage_source;
                    break;
                case 'G':
                    this->type = ct_voltage_controlled_current_source;
                    break;
                case 'H':
                    this->type = ct_current_controlled_current_source;
                    break;
            }
        }
        
        component_types get_type() const
        {
            return type;
        }

        unsigned int element_size() const
        {
            switch(type)
            {
                case ct_resistor:
                case ct_capacitor:
                case ct_inductor:
                case ct_voltage_source:
                case ct_opamp:
                case ct_voltage_controlled_voltage_source:
                case ct_current_controlled_current_source:
                    return 1;
                    break;
                case ct_current_controlled_voltage_source:
                    return 2;
                    break;
                case ct_current_source:
                case ct_voltage_controlled_current_source:
                    return 0;
                    break;
                case ct_none: // suppress warning
                    break;
            }
            // suppress warning
            return 0;
        }

        const std::vector<unsigned int>& get_nodes() const
        {
            return nodes;
        }

        GiNaC::ex get_value() const
        {
            GiNaC::ex s = get_symbol("s");
            switch(type)
            {
                case ct_resistor:
                    return 1 / value;
                    break;
                case ct_capacitor:
                    return s * value;
                    break;
                case ct_inductor:
                    return 1 / (s * value);
                    break;
                case ct_voltage_source:
                case ct_current_source:
                case ct_opamp:
                case ct_voltage_controlled_voltage_source:
                case ct_current_controlled_voltage_source:
                case ct_voltage_controlled_current_source:
                case ct_current_controlled_current_source:
                    return value;
                    break;
                case ct_none: // suppress warning
                    break;
            }
            // suppress warning
            return GiNaC::ex();
        }

    protected:
        component_types type;
        std::vector<unsigned int> nodes;
        GiNaC::ex value;
};

#endif //COMPONENT_HPP
