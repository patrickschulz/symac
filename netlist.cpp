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
static bool is_subcircuit(const std::string& line)
{
    return line[0] == '.';
}
static bool is_two_terminal_device(const std::string& line)
{
    std::regex rx(R"(^\s*[VIRCL]\s+\w+\s+\w+\s+\w+\s*$)");
    return std::regex_search(line, rx);
}

static bool is_three_terminal_device(const std::string& line)
{
    std::regex rx(R"(^\s*[O]\s+\w+\s+\w+\s+\w+\s*$)");
    return std::regex_match(line, rx);
}

static bool is_four_terminal_device(const std::string& line)
{
    std::regex rx(R"(^\s*[EFGH]\s+\w+\s+\w+\s+\w+\s+\w+\s+\w+\s*$)");
    return std::regex_match(line, rx);
}

static bool is_component(const std::string& line)
{
    return is_two_terminal_device(line) || is_three_terminal_device(line) || is_four_terminal_device(line);
}

unsigned int netlist::numbr_terminals(char type)
{
    unsigned int number_terminals;
   switch(type)
    {
        case 'R':
            return number_terminals = 2;
        case 'C':
            return number_terminals = 2;
        case 'L':
            return number_terminals = 2;
        case 'V':
            return number_terminals = 2;
        case 'I':
            return number_terminals = 2;
        case 'E':
            return number_terminals = 4;
        case 'O':
            return number_terminals = 3;
        case 'F':
            return number_terminals = 4;
        case 'G':
            return number_terminals = 4;
        case 'H':
            return number_terminals = 4;
        default:
            std::cerr << "Unknown component: '" << type << "'\n";
            return 0   ;
    }
    
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

// std::string netlist::to_string(std::vector<unsigned int> n)
// {
//     std::stringstream v ;
//     std::string s;
//     copy(n.begin(),n.end(), ostream_iterator<unsigned int> (v," ");    
//     s = v.str();
//     return s;
// }
void netlist::read(std::string filename)
{
    unsigned int number_subckt=0 ;
    std::ifstream file(filename);
    output_map.insert(std::make_pair(0 , "GND")).first -> second;
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
            component_read_in(line);
        }
        else if(is_subcircuit(line))
        {
            std::istringstream stream(line);
            char dot;
            std::string v;
            
            stream >> dot;
            stream >> v;
            if (v=="subckt")
            { 
                std::vector<std::string> sub_terminal_names;
                std::string subcktname;
                std::string tname;
                stream >> subcktname;
                while(stream >> tname)
                {
                    sub_terminal_names.push_back(tname);
                }
                
                subckt_vector.push_back(subcircuit(subcktname, sub_terminal_names));
            }
            else if (v=="[A-Z]")
            {
                line.erase(line.begin()); //delete the dot
                subckt_vector.at(number_subckt).add_line(line); //save it in vector in object
            }
            else if (v=="end")
            { 
                number_subckt++;
            }
            else
            {
                std::string terminal_name;
                unsigned int id;
                
                std::vector<std::string> sub_line;
//              Check the possibly multiple subckts for the specific one
                for (unsigned int i = 0; i<subckt_vector.size();i++)
                {
                    std::string name = subckt_vector.at(i).get_name();
                    if ( name == v)
                    {
                        id = i;
                    }
                }
                
                subckt_vector.at(id).change_lines();
                std::vector<std::string> sub_terminal_names = subckt_vector.at(number_subckt).get_terminals();
                std::vector<std::string> slines =  subckt_vector.at(number_subckt).get_sublines();
                for(unsigned int i = 0 ; i< sub_terminal_names.size();i++)
                {
                    stream >> terminal_name;
                    
                    std::string sub_name = sub_terminal_names[i];
                    auto pos = std::find(slines.begin(),slines.end(), sub_name);
                    for (;pos != slines.end();)
                    {
                        slines[pos-slines.begin()]= terminal_name;
                        pos=std::find(slines.begin(),slines.end(), sub_name);
                        
                    }
                    i++;
                }
                for(unsigned int i=0; i<slines.size();i++)
                {
                    component_read_in(slines[i]);
                }
            }    
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
    nodes = nmap.get_number_nodes();
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

void netlist::add_to_output_map(unsigned int unode, std::string snode)
{
    auto it = output_map.find(unode);
    if ( it == output_map.end())
    {
        output_map.insert(std::make_pair(unode, snode)).first ->second;
    }
}
std::string netlist::get_output_node(unsigned int unode) const
{
    std::string snode = "Not found";
    auto it = output_map.find(unode);
    if (it != output_map.end())
    {
        snode = it -> second;
    }    
    return snode;
}
void netlist::component_read_in(const std::string& line)
{
    GiNaC::ex value;
    std::string snode;
    std::istringstream stream(line);
    
            
    char type;
    stream >> type;
    unsigned int number_terminals = numbr_terminals(type);
    std::vector<unsigned int> nodes;
    for (unsigned int i = 0; i < number_terminals; i++)
    {
        unsigned int n;
        stream >> snode;
        nmap.add_to_map(snode);
        n = nmap.get_map_node();
        nodes.push_back(std::move(n));
        add_to_output_map(n,snode);                
    }
    std::string v;
    stream >> v;
    if(v.size() > 0 && v.find_first_not_of("0123456789.-") == std::string::npos) // is the string a numeric?
    {
        value = std::stod(v);
        
    }
    else
    {
        value = get_symbol(v);
    }
    components.push_back(create_component(type,nodes,value));
}






