#include "netlist.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

static bool is_comment(char type)
{
    return type == '*';
}

static bool is_two_terminal_device(char type)
{
    const char component_types[5] = {
        'V', 'I', 'R', 'C', 'L'
    };
    return std::find(std::begin(component_types), std::end(component_types), type) != std::end(component_types);
}

static bool is_three_terminal_device(char type)
{
    const char component_types[1] = {
        'O'
    };
    return std::find(std::begin(component_types), std::end(component_types), type) != std::end(component_types);
}

static bool is_four_terminal_device(char type)
{
    const char component_types[4] = {
        'E', 'F', 'G', 'H'
    };
    return std::find(std::begin(component_types), std::end(component_types), type) != std::end(component_types);
}

static bool is_component(char type)
{
    return is_two_terminal_device(type) || is_three_terminal_device(type) || is_four_terminal_device(type);
}

netlist::netlist() :
    valid(false),
    numnodes(0),
    numsources(0),
    numimpedances(0),
    numopamps(0),
    numvcvs(0),
    numccvs(0),
    numcccs(0),

    current_impedance(1),
    current_source(1),
    current_opamp(1),
    current_vcvs(1),
    current_ccvs(1),
    current_cccs(1)
{
}

netlist::netlist(const std::string& filename) :
    netlist()
{
    read(filename);
}

void netlist::add_component(std::unique_ptr<component>&& comp)
{
    components.push_back(std::move(comp));
    reset();
    update();
    for(const auto& c : components)
    {
        c->set_stamp(*this);
    }
}

void netlist::read(std::string filename)
{
    std::ifstream file(filename);
    if(!file.is_open())
    {
        std::stringstream str;
        str << "could not open netlist '" << filename << '\'';
        valid = false;
        return;
    }

    bool title_found = false;
    while(true)
    {
        std::string line;
        std::getline(file, line);
        if(!file)
        {
            break;
        }
        // TODO: check for empty line
        // TODO: strip leading whitespace
        if(is_comment(line[0])); // ignore
        else if(is_component(line[0]))
        {
            components.push_back(create_component(line));
        }
        else 
        {
            if(title_found)
            {
                std::cerr << "unknown line: '" << line << "'\n";
            }
            else
            {
                title_found = true;
            }
        }
    }
    // set number of nodes, number of sources etc.
    update();
    for(const auto& c : components)
    {
        c->set_stamp(*this);
    }
    valid = true;
}

const std::vector<const component*> netlist::get_devices(component_types type) const
{
    std::vector<const component*> devices;
    for(const auto& c : components)
    {
        if(c->type() & type)
        {
            devices.emplace_back(c.get());
        }
    }
    return devices;
}

unsigned int netlist::number_of_devices(component_types type) const
{
    const std::vector<const component*> devices = get_devices(type);
    return devices.size();
}

unsigned int netlist::number_of_impedances() const
{
    return number_of_devices(ct_resistor) + number_of_devices(ct_capacitor) + number_of_devices(ct_inductor);
}

unsigned int netlist::number_of_nodes() const
{
    unsigned int nodes = 0;
    for(const auto& com: components)
    {
        nodes = std::max(nodes, com->get_max_node());
    }
    return nodes;
}

int netlist::number_of_voltage_sources() const
{
    int sources = 0;
    for(const auto& com: components)
    {
        if(com->type() == component_types::ct_voltage_source)
        {
            ++sources;
        }
    }
    return sources;
}

unsigned int netlist::full_network_size() const
{
    return numnodes       + 
           numsources     + 
           numimpedances  + 
           numopamps      + 
           numvcvs        +
           numccvs * 2    +
           numcccs;
}

void netlist::print_all_components() const
{
    for(const auto& component : components)
    {
        std::cout << component->to_string() << '\n';
    }
}

netlist::operator bool()
{
    return valid;
}

void netlist::reset()
{
    for(const auto& c : components)
    {
        c->reset_stamp();
    }
    current_impedance = 1;
    current_source = 1;
    current_opamp = 1;
    current_vcvs = 1;
    current_ccvs = 1;
    current_cccs = 1;
}

void netlist::update()
{
    numnodes = number_of_nodes();
    numsources     = number_of_devices(ct_voltage_source);
    numimpedances  = number_of_devices(ct_resistor) + number_of_devices(ct_capacitor) + number_of_devices(ct_inductor);
    numopamps      = number_of_devices(ct_opamp);
    numvcvs        = number_of_devices(ct_voltage_controlled_voltage_source);
    numccvs        = number_of_devices(ct_current_controlled_voltage_source);
    numcccs        = number_of_devices(ct_current_controlled_current_source);
}

