#include "component.hpp"

#include <sstream>

#include "symbol.hpp"

component::component(const std::string& name, component_types type, const std::vector<std::string>& nodes, const GiNaC::symbol& value) :
    name(name), type(type), nodes(nodes), value(value)
{   
    switch(type)
    {
        case ct_resistor:
        case ct_capacitor:
        case ct_inductor:
        case ct_voltage_source:
        case ct_current_source:
        case ct_voltage_controlled_voltage_source:
        case ct_current_controlled_current_source:
            mna_size = 1;
            terminals.push_back(".p");
            break;
        case ct_opamp:
            mna_size = 1;
            terminals.push_back(".out");
            break;
        case ct_current_controlled_voltage_source:
            mna_size = 2;
            terminals.push_back(".p");
            terminals.push_back(".cp");
            break;
        case ct_voltage_controlled_current_source:
            mna_size = 0;
            break;
    }
}

std::string component::get_name() const
{
    return name;
}

std::vector<std::string> component::get_terminal_names() const
{
    return terminals;
}

void component::name_prepend(const std::string& prefix)
{
    name = prefix + name;
}

unsigned int component::element_size() const
{
    return mna_size;
}

const component_types& component::get_type() const
{
    return type;
}

void component::set_type(const component_types& t) 
{
    type = t;
}

const std::vector<std::string>& component::get_nodes() const
{
    return nodes;
}

void component::set_nodes(const std::vector<std::string>& n)
{
    nodes = n;
}

const GiNaC::symbol& component::get_value() const
{
    return value;
}

void component::set_value(const GiNaC::symbol& v)
{
    value = v;
}

std::ostream& operator<<(std::ostream& stream, const component& c)
{
    stream << c.type << " (" << c.value << ") { ";
    for(auto n : c.nodes)
    {
        stream << n << ' ';
    }
    stream << '}';
    return stream;
}


// vim: nowrap
