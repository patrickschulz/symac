#include "component.hpp"

#include <sstream>

#include "symbol.hpp"

component::component(const std::string& name, char chtype, const std::vector<std::string>& nodes, const GiNaC::symbol& value) :
    name(name), nodes(nodes), value(value)
{   
    switch(chtype)
    {
        case 'R':
            type = ct_resistor;
            mna_size = 1;
            terminals.push_back(".p");
            break;
        case 'C':
            type = ct_capacitor;
            mna_size = 1;
            terminals.push_back(".p");
            break;
        case 'L':
            type = ct_inductor;
            mna_size = 1;
            terminals.push_back(".p");
            break;
        case 'V':
            type = ct_voltage_source;
            mna_size = 1;
            terminals.push_back(".p");
            break;
        case 'I':
            type = ct_current_source;
            mna_size = 1;
            terminals.push_back(".p");
            break;
        case 'O':
            type = ct_opamp;
            mna_size = 1;
            terminals.push_back(".out");
            break;
        case 'E':
            type = ct_voltage_controlled_voltage_source;
            mna_size = 1;
            terminals.push_back(".p");
            break;
        case 'F':
            type = ct_current_controlled_voltage_source;
            mna_size = 2;
            terminals.push_back(".p");
            terminals.push_back(".cp");
            break;
        case 'G':
            type = ct_voltage_controlled_current_source;
            mna_size = 0;
            break;
        case 'H':
            type = ct_current_controlled_current_source;
            mna_size = 1;
            terminals.push_back(".p");
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

const std::vector<std::string>& component::get_nodes() const
{
    return nodes;
}

void component::set_nodes(const std::vector<std::string>& nodes)
{
    this->nodes = nodes;
}

std::string component::to_string() const
{
    std::ostringstream stream;
    stream << type << " (" << value << ") ";
    stream << "{ ";
    for(auto node : nodes)
    {
        stream << node << ' ';
    }
    stream << '}';
    return stream.str();
}

GiNaC::ex component::get_value() const
{
    return value;
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
