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
static bool is_subckt_line(std::string s)
{
    std::regex rx("[A-Z]");
    return std::regex_match(s,rx);
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
    valid(false)
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
                    sub_terminal_names.push_back(std::move (tname));
                }
                subcircuit subckt(subcktname,sub_terminal_names);
                subckt_vector.push_back(subckt);
            }
            else if (is_subckt_line(v))
            {
                line.erase(line.begin()); //delete the dot
                std::vector <std::string> terminals = subckt_vector.at(number_subckt).get_terminals();
                std::string changed_sub_line = change_sub_line(line,terminals);// maybe in netlist?
                subckt_vector.at(number_subckt).add_line(changed_sub_line); //save it in vector in object                
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
                std::vector<std::string> sub_terminal_names = subckt_vector.at(id).get_terminals();
                std::vector<std::string> slines =  subckt_vector.at(id).get_sublines();
                unsigned int tnumber = 0;
                while (stream >> terminal_name)
                {
                    
                    for(unsigned int i = 0 ; i< slines.size();i++)
                    {
                        std::string sline = slines[i];
                        size_t pos = sline.find(sub_terminal_names[tnumber]);
                        while ( pos != std::string::npos)
                        {
                            sline.replace(pos, 1, terminal_name);
                            pos = sline.find(sub_terminal_names[tnumber], pos + 1);
                        }
                        slines[i] = sline;
                    }
                    tnumber++;
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
    return state.full_size();
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
    state.reset();
}

void netlist::update()
{
    state.numnodes       = number_of_nodes();
    state.numsources     = number_of_devices(ct_voltage_source);
    state.numimpedances  = number_of_devices(ct_resistor) + number_of_devices(ct_capacitor) + number_of_devices(ct_inductor);
    state.numopamps      = number_of_devices(ct_opamp);
    state.numvcvs        = number_of_devices(ct_voltage_controlled_voltage_source);
    state.numccvs        = number_of_devices(ct_current_controlled_voltage_source);
    state.numcccs        = number_of_devices(ct_current_controlled_current_source);
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
    std::istringstream stream(line);
            
    char type;
    stream >> type;
    unsigned int number_terminals = numbr_terminals(type);
    std::vector<unsigned int> nodes;
    for (unsigned int i = 0; i < number_terminals; i++)
    {
        std::string snode;
        stream >> snode;
        unsigned int n = nmap.get_mapped_node(snode);
        nodes.emplace_back(n);
        add_to_output_map(n, snode);                
    }
    std::string v;
    stream >> v;
    GiNaC::ex value;
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

std::string netlist::change_sub_line(std::string line, std::vector<std::string> terminals)
{
        std::stringstream stream (line);
        std::string oline;
        char type;
        std::string buf;
        
        stream >> type;
        oline= type;
        oline.append(" ");
        unsigned int nterminals = numbr_terminals(type);
        
        for (unsigned int i = 0 ; i< nterminals; i++)
        {   
            
            stream >> buf;
            if(std::find(terminals.begin(),terminals.end(), buf) == terminals.end())
            {
                
                unsigned int unode = std::stoi(buf);
                unode += 42 ;
                std::string snode = to_String(unode);
                oline.append(snode);
                oline.append(" ");
            }
            else
            {
                oline.append(buf);
                oline.append(" ");
                
            }
        }
        stream >> buf;
        oline += buf;
        return oline;
}

std::string netlist::to_String(unsigned int u)
{
    std::ostringstream stream;
    stream << u;
    return stream.str();
}

