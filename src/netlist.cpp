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

/*
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
*/

/*
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
*/

unsigned int netlist::number_terminals(char type)
{
    switch(type)
    {
        case 'R': // resistor
            return 2;
        case 'C': // capacitor
            return 2;
        case 'L': // inductor
            return 2;
        case 'V': // voltage_source
            return 2;
        case 'I': // current_source
            return 2;
        case 'O': // opamp
            return 3;
        case 'E': // voltage controlled voltage source
            return 4;
        case 'F': // current controlled voltage source
            return 4;
        case 'G': // voltage_controlled_current_source
            return 4;
        case 'H': // current_controlled_current_source
            return 4;
        default:
            std::cerr << "Unknown component: '" << type << "'\n";
            return 0;
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

void netlist::add_component(const component& c)
{
    components.add_component(c);
}

void netlist::read(std::string filename)
{
    std::ifstream file(filename);
    if(!file.is_open())
    {
        valid = false;
        return;
    }

    bool title_found = false;
    bool idle = true;
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
        if(line.empty() || is_comment(line))
        {
            continue;
        }
        if(idle)
        {
            if(line.find(".subckt") != std::string::npos)
            {
                subcircuit s = read_subcircuit_header(line);
                subcircuits[s.get_name()] = s;
                current_subcircuit = &subcircuits[s.get_name()];
                idle = false;
            }
            else if(line.find("X") != std::string::npos)
            {
                /*
                vector<component> cs = translate_subcircuit(line, nlist);
                for(auto c : cs)
                {
                    nlist.components.push_back(c);
                }
                */
            }
            //else if(is_subckt_end(line))
            //{
            //    subckt = false;
            //    number_subckt++;
            //    continue;
            //}
            //if(subckt)
            //{
            //    read_subckt_line(line, number_subckt);
            //}
            //else if(is_subckt_title(line))
            //{
            //    subckt = true;
            //    std::string init (".subckt");
            //    std::string title = line;
            //    title.erase(0,init.length());
            //    read_subckt_title(title);
            //}
            //else if(is_subckt_call(line))
            //{
            //    subckt_call(line);
            //}
            //else if (is_command_simplify(line))
            //{
            //    save_simpl_line(line);
            //    simplification = true;
            //}
            else if(is_component(line))
            {
                components.add_component(component_read_in(line));
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
        else // !idle (reading-in subcircuit definitions
        {
            if(line.find(".end") != std::string::npos)
            {
                idle = true;
            }
            else
            {
                component c = component_read_in(line);
                current_subcircuit->add_component(c);
            }
        }
    }
    valid = true;
}

netlist::operator bool()
{
    return valid;
}

std::string netlist::get_output_node(unsigned int unode) const
{
    return nmap[unode];
}

unsigned int netlist::get_unode(const std::string& snode) const
{
    if(nmap.check_node(snode))
    {
        return nmap[snode];
    }
    else
    {
        return 0;
    }
}
    
component netlist::component_read_in(const std::string& line)
{
    std::istringstream stream(line);
    std::string name;
    stream >> name;
    char type = name[0];
    unsigned int nterminals = number_terminals(type);
    std::vector<std::string> nodes;
    std::string snode;
    for (unsigned int i = 0; i < nterminals; i++)
    {
        stream >> snode;
        //unsigned int inode = nmap[snode];
        nodes.push_back(snode);
    }
    std::string v;
    stream >> v;
    //simpl_map.insert(std::make_pair(v, 0));
    
    GiNaC::ex value;
    if(v.size() > 0 && v.find_first_not_of("0123456789.-") == std::string::npos) // is the string a numeric?
    {
        value = std::stod(v);
    }
    else
    {
        value = get_symbol(v);
    }
    return component(type, nodes, value);
}

/*
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
        unsigned int nterminals = number_terminals(type);
        
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
        name_map[sub_t_names[i]] = t_names[i];
    }
    
    std::stringstream stream(sline);
    std::string buf;
    std::string oline;
    stream >> buf;
    char type=buf[0];
    oline = buf;
    oline.append(" ");
    unsigned int nterminals = number_terminals(type);
    std::map<std::string,std::string>::iterator it;
    for(unsigned int j = 0; j< nterminals; j++)
    {
        stream >> buf;
        it = name_map.find(buf);
        if(it != name_map.end())
        {
            buf = it->second;
        }
        oline.append(buf);
        oline.append(" ");
    }
    stream >> buf;
    oline.append(buf);
    return oline;
}
*/

/*
std::vector<std::string> netlist::get_values()
{
    std::vector<std::string> values;
    for(const component& c : components)
    {
        std::ostringstream stream;
        stream << c.get_value();
        values.push_back(stream.str());
    }
    return values;
}
*/

/*
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
    unsigned int val_second= it_second->second;
    unsigned int val_first = val_second + 1;
    it_first->second = val_first;
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
*/
