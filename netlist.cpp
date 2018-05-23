#include "netlist.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <ginac/ginac.h>

#include "util.hpp"

static bool is_comment(const std::string& line)
{
    return line[0] == '*';
}

static bool is_two_terminal_device(const std::string& line)
{
    std::regex rx(R"(^\s*[VIRCL]\s+\d+\s+\d+\s+\w+\s*$)");
    return std::regex_search(line, rx);
}

static bool is_three_terminal_device(const std::string& line)
{
    std::regex rx(R"(^\s*[O]\s+\d+\s+\d+\s+\d+\s*$)");
    return std::regex_match(line, rx);
}

static bool is_four_terminal_device(const std::string& line)
{
    std::regex rx(R"(^\s*[EFGH]\s+\d+\s+\d+\s+\d+\s+\d+\s+\w+\s*$)");
    return std::regex_match(line, rx);
}

static bool is_component(const std::string& line)
{
    return is_two_terminal_device(line) || is_three_terminal_device(line) || is_four_terminal_device(line);
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

std::string netlist::to_string(std::vector<unsigned int> n)
{
    std::stringstream v ;
    std::string s;
    copy(n.begin(),n.end(), ostream_iterator<unsigned int> (v," ");    
    s = v.str();
    return s;
}
void netlist::read(std::string filename)
{
    std::ifstream file(filename);
    std::map<std::string, unsigned int> map.nodemap;
    if(!file.is_open())
    {
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
        trim(line); // remove whitespace
        if(line.empty())
        {
            continue;
        }
        if(is_comment(line)); // ignore
        else if(is_component(line))
        {
            
            GiNaC::ex value;
            std::istringstream stream(line);
            std::vector<unsigned int> nodes;
            char type;
            stream >> type;
            switch()//type ?
            {
                case is_two_terminal_device     :   number_terminals = 2;
                case is_three_terminal_device   :   number_terminals = 3;
                case is_four_terminal_device    :   number_terminals = 4;
            }
            //-> give it to map functions
            my_map.add_to_map(number_terminals,line,nodemap);
            nodes = nodemap.get_calc_nodes();
            value = nodemap.get_map_value();
//          kind of zip it ? 
//          std::tuple<char,unsigned int,GiNaC::ex> comp_arg= std::make_tuple(type, nodes, value); 
//                          
            std::string comp_arg = type + to_string(nodes) + value;
            components.push_back(create_component(comp_arg));
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
    numnodes       = number_of_nodes();
    numsources     = number_of_devices(ct_voltage_source);
    numimpedances  = number_of_devices(ct_resistor) + number_of_devices(ct_capacitor) + number_of_devices(ct_inductor);
    numopamps      = number_of_devices(ct_opamp);
    numvcvs        = number_of_devices(ct_voltage_controlled_voltage_source);
    numccvs        = number_of_devices(ct_current_controlled_voltage_source);
    numcccs        = number_of_devices(ct_current_controlled_current_source);
}

