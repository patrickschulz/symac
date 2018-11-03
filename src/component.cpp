#include "component.hpp"

#include <sstream>

#include "symbol.hpp"

component::component(const component_proxy& p) :
    component(p.name, p.type, p.nodes, get_symbol(p.value))
{
}

/*
 * TODO: this is needed if numeric values should be supported (they should!)
 * However, for this the components need to store a GiNaC::ex, not a GiNaC::symbol as value
static GiNaC::ex check_and_convert_numeric_symbol(const std::string& v)
{
    GiNaC::ex value;
    if(v.size() > 0 && v.find_first_not_of("0123456789.-") == std::string::npos) // is the string a numeric?
    {
        value = std::stod(v);
    }
    else
    {
        value = get_symbol(v);
    }
    return value;
}
*/

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

// vim: nowrap
