#include "component.hpp"

#include "symbol.hpp"

component::component(const std::string& name, component_types type, const std::vector<std::string>& nodes, const GiNaC::ex& value) :
    name(name), type(type), nodes(nodes), value(value)
{   }

component::component(const std::string& name, char type, const std::vector<std::string>& nodes, const GiNaC::ex& value) :
    name(name), nodes(nodes), value(value)
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

std::string component::get_name() const
{
    return name;
}

std::vector<std::string> component::get_terminal_names() const
{
    std::vector<std::string> terminals;
    switch(type)
    {
        case ct_resistor:
        case ct_capacitor:
        case ct_inductor:
        case ct_voltage_source:
        case ct_voltage_controlled_voltage_source:
            terminals.push_back(".p");
            break;
        case ct_current_controlled_current_source:
            terminals.push_back(".p");
            break;
        case ct_opamp: 
            terminals.push_back(".out");
            break;
        case ct_current_controlled_voltage_source:
            terminals.push_back(".p");
            terminals.push_back(".cp");
            break;
        // these have no currents in the results matrix, so the vector is empty
        case ct_current_source:
        case ct_voltage_controlled_current_source:
        case ct_none:
            break;
    }
    return terminals;
}

void component::name_prepend(const std::string& prefix)
{
    name = prefix + name;
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
    return value;
}

// vim: nowrap
