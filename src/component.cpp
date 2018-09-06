#include "component.hpp"

#include "symbol.hpp"

component::component(component_types type, const std::vector<std::string>& nodes, const GiNaC::ex& value) :
    type(type), nodes(nodes), value(value)
{   }

component::component(char type, const std::vector<std::string>& nodes, const GiNaC::ex& value) :
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

component_types component::get_type() const
{
    return type;
}

unsigned int component::element_size() const
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

const std::vector<std::string>& component::get_nodes() const
{
    return nodes;
}

void component::set_nodes(const std::vector<std::string>& nodes)
{
    this->nodes = nodes;
}

GiNaC::ex component::get_value() const
{
    GiNaC::ex s = get_symbol("s");
    switch(type)
    {
        case ct_resistor:
            //return 1 / value;
            return value;
            break;
        case ct_capacitor:
            //return s * value;
            return 1 / (s * value);
            break;
        case ct_inductor:
            //return 1 / (s * value);
            return s * value;
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
    // suppress warning, this line should never be reached
    return GiNaC::ex();
}

// vim: nowrap
