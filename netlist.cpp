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

static bool is_subckt_title(const std::string& line)
{
    std::regex rx(R"(^\.subckt)");
    return std::regex_search(line, rx);
}

static bool is_subckt_end(const std::string& line)
{
    std::regex rx(R"(^\.end)");
    return std::regex_search(line, rx);
}

bool netlist::is_subckt_call(const std::string& line)
{
    std::regex rx(R"(^X\w+)");
    return std::regex_search(line, rx);
}

static bool is_command_simplify(const std::string& line)
{
    if(line.find("simplify") != std::string::npos)
    {   
        return true;
    }
    else
    {
        return false;
    }
}

unsigned int netlist::numbr_terminals(char type)
{
    unsigned int number_terminals;
    switch(type)
    {
        case 'R': // resistor
            return number_terminals = 2;
        case 'C': // capacitor
            return number_terminals = 2;
        case 'L': // inductor
            return number_terminals = 2;
        case 'V': // voltage_source
            return number_terminals = 2;
        case 'I': // current_source
            return number_terminals = 2;
        case 'O': // opamp
            return number_terminals = 3;
        case 'E': // voltage controlled voltage source
            return number_terminals = 4;
        case 'F': // current controlled voltage source
            return number_terminals = 4;
        case 'G': // voltage_controlled_current_source
            return number_terminals = 4;
        case 'H': // current_controlled_current_source
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
    for(unsigned int i = 0; i < components.size(); ++i)
    {
        components[i]->set_stamp(i + number_of_nodes());
    }
}

void netlist::read(std::string filename)
{
    std::ifstream file(filename);
    output_map.insert(std::make_pair(0 , "GND"));
    if(!file.is_open())
    {
        valid = false;
        return;
    }

    bool title_found = false;
    bool subckt = false;
    unsigned int number_subckt=0;
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
        if(is_comment(line))
        {
            continue;
        } // ignore
        else if(is_subckt_end(line))
        {
            subckt = false;
            number_subckt++;
            continue;
        }
        if(subckt)
        {
            read_subckt_line(line, number_subckt);
        }
        else if(is_subckt_title(line))
        {
            subckt = true;
            std::string init (".subckt");
            std::string title = line;
            title.erase(0,init.length());
            read_subckt_title(title);
        }
        else if(is_subckt_call(line))
        {
            subckt_call(line);
        }
        else if (is_command_simplify(line))
        {
            save_simpl_line(line);
            simplification = true;
        }
        else if(is_component(line))
        {
            component_read_in(line);
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
    for(unsigned int i = 0; i < components.size(); ++i)
    {
        components[i]->set_stamp(i + number_of_nodes());
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
    return number_of_nodes() 
         + number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source | ct_voltage_controlled_voltage_source | ct_current_controlled_current_source) 
         + 2 * number_of_devices(ct_current_controlled_current_source);
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

unsigned int netlist::get_unode( std::string snode) const
{
    unsigned int unode = nmap.find_node(snode);
    return unode;
}
    
void netlist::component_read_in(const std::string& line)
{
    std::istringstream stream(line);
    std::string name;
    stream >> name;
    char type = name[0];
    unsigned int number_terminals = numbr_terminals(type);
    std::vector<unsigned int> nodes;
    std::string snode;
    for (unsigned int i = 0; i < number_terminals; i++)
    {
        stream >> snode;
        unsigned int n = nmap.get_map_node(snode);
        nodes.push_back(std::move(n));
        add_to_output_map(n,snode);                
    }
    std::string v;
    stream >> v;
    set_matlab_values(v);
    simpl_map.insert(std::make_pair(v, 0)).first ->second;
    
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

// Subcircuit-Functs
void netlist::subckt_call(const std::string& line)
{
    std::vector<std::string> terminal_names;
    std::string subckt_value;
    std::stringstream stream(line);
    char dot;
    stream >> dot;
    std::string v;
    stream >> v;
    //              Check the possibly multiple subckts for the specific one
    unsigned int id;
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
    std::string terminal_name;
    for (unsigned int i = 0; i<sub_terminal_names.size();i++)
    {
        stream >> terminal_name;
        terminal_names.push_back(terminal_name);
    }
    stream >> subckt_value;
    for (unsigned int i = 0; i < slines.size(); i++)
    {
        std::string sline = slines.at(i);
        std::string ch_sline= change_subline_nodes(sline, sub_terminal_names, subckt_value);
        ch_sline = change_subline_terminals(sline,sub_terminal_names,terminal_names);
        slines[i] = ch_sline;
    }
    for(unsigned int i=0; i<slines.size();i++)
    {
        component_read_in(slines[i]);
    }
}
void netlist::read_subckt_line(const std::string& line, unsigned int number_subckt)
{
    std::string sub_line = line;
    subckt_vector.at(number_subckt).add_line(sub_line); //save it in vector in object                

}
void netlist::read_subckt_title(std::string& title)
{
    std::stringstream stream(title);
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

std::string netlist::change_subline_nodes(std::string line, std::vector<std::string> terminals,std::string subckt_value)
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
                if ( buf == "0" || buf == "GND" )
                {
                    oline.append(buf);
                    oline.append(" ");
                }
                else
                {
                    oline.append(subckt_value);
                    oline.append(":");
                    oline.append(buf);
                    oline.append(" ");
                }
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
std::string netlist::change_subline_terminals(std::string sline, std::vector<std::string> sub_t_names, std::vector<std::string> t_names)
{
    //create map of sub_t_names & t_names 
    std::map<std::string,std::string> name_map;
    for (unsigned int i = 0 ; i< sub_t_names.size();i++)
    {
        name_map[sub_t_names[i]]=t_names[i];
    }
    
    std::stringstream stream(sline);
    std::string buf;
    std::string oline;
    stream >> buf;
    char type=buf[0];
    oline = buf;
    oline.append(" ");
    unsigned int nterminals = numbr_terminals(type);
    std::map<std::string,std::string>::iterator it;
    for(unsigned int j = 0; j< nterminals; j++)
    {
        stream >> buf;
        it = name_map.find(buf);
        if(it != name_map.end())
        {
            buf = it -> second;
        }
        oline.append(buf);
        oline.append(" ");
    }
    stream >> buf;
    oline.append(buf);
    return oline;
}

//Matlab Values for default settings
std::vector<std::string> netlist::get_values()
{
    return values;
}
void netlist::set_matlab_values(std::string v)
{
    values.push_back(v);
}
// simplification
void netlist::save_simpl_line(const std::string& line)
{
    simplify_lines.push_back(line);
}
void netlist::set_simplification()
{
    for (unsigned int i = 0; i < simplify_lines.size(); i++)
    {
        std::string line = simplify_lines[i];
        std::stringstream stream (line);
        std::string first;
        std::string second;
        stream >> first;//s = simplify
        stream >> first;// s = R1
        stream >> second;// s = >>
        stream >> second;// s = R2
        change_simpl_map(first, second);
    }
}
std::map<std::string, unsigned int > netlist::get_simplifications()
{
    return simpl_map;
}
bool netlist::is_simplification()
{
    return simplification;
}
void netlist::change_simpl_map(std::string greater, std::string smaller)
{
    auto it_first = simpl_map.find(greater);
    auto it_second= simpl_map.find(smaller);
    unsigned int val_second= it_second -> second;
    unsigned int val_first = val_second + 1;
    it_first -> second = val_first;
}
void netlist::set_simpl_level(const std::string& line)
{
    std::stringstream stream(line);
    std::string s;
    stream >> s ;// command "Simplify Level"
    stream >> simpl_level;
}
std::string netlist::get_simpl_level()
{
    return simpl_level;
}

