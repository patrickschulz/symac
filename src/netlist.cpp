#include "netlist.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <ginac/ginac.h>

#include "util.hpp"
#include "symbol.hpp"

static bool is_comment(const std::string& line)
{
    return line[0] == '*';
}

static bool is_two_terminal_device(const std::string& line)
{
    std::regex rx(R"(^\s*[VIRCL]\w*\s+\w+\s+\w+\s+\w+\s*$)");
    return std::regex_search(line, rx);
}

static bool is_three_terminal_device(const std::string& line)
{
    std::regex rx(R"(^\s*[O]\w*\s+\w+\s+\w+\s+\w+\s*$)");
    return std::regex_match(line, rx);
}

static bool is_four_terminal_device(const std::string& line)
{
    std::regex rx(R"(^\s*[EFGH]\w*\s+\w+\s+\w+\s+\w+\s+\w+\s+\w+\s*$)");
    return std::regex_match(line, rx);
}

static bool is_component(const std::string& line)
{
    return is_two_terminal_device(line) || is_three_terminal_device(line) || is_four_terminal_device(line);
}

static bool is_command_simplify(const std::string& line)
{
    return line.find("simplify") != std::string::npos;
}

unsigned int netlist::number_terminals(char type)
{
    unsigned int num = 0;
    switch(type)
    {
        case 'R': // resistor
        case 'C': // capacitor
        case 'L': // inductor
        case 'V': // voltage_source
        case 'I': // current_source
            num = 2;
            break;
        case 'O': // opamp
            num = 3;
            break;
        case 'E': // voltage controlled voltage source
        case 'F': // current controlled voltage source
        case 'G': // voltage_controlled_current_source
        case 'H': // current_controlled_current_source
            num = 4;
            break;
        default:
            std::cerr << "Unknown component: '" << type << "'\n";
            return 0;
    }
    return num;
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

void netlist::translate_subcircuit(const std::string& line)
{
    std::istringstream stream(line);
    std::string subname;
    char X;
    stream >> X >> subname;

    // save instance nodes
    std::vector<std::string> nodes;
    std::copy(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>(), std::back_inserter(nodes));

    // get mapped components and store them
    subcircuit sub = subcircuits[subname];
    std::vector<component> subcomponents = sub.get_mapped_components(nodes);
    for(const component& c : subcomponents)
    {
        components.add_component(c);
    }
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
            if(line.find(".print") != std::string::npos)
            {

            }
            else if(line.find(".subckt") != std::string::npos)
            {
                subcircuit s = read_subcircuit_header(line);
                subcircuits[s.get_name()] = s;
                current_subcircuit = &subcircuits[s.get_name()];
                idle = false;
            }
            else if(line.find("X") != std::string::npos)
            {
                translate_subcircuit(line);
            }
            else if (is_command_simplify(line))
            {
                //save_simpl_line(line);
                //simplification = true;
            }
            else if(is_component(line))
            {
                components.add_component(component_read_in(line));
            }
            else
            {
                if(title_found)
                {
                    std::cerr << "unknown line: '" << line << "'\n";
                    valid = false;
                    return;
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
        nodes.push_back(snode);
    }
    std::string v;
    stream >> v;
    //simpl_map.insert(std::make_pair(v, 0));
    
    GiNaC::ex value = check_and_convert_numeric_symbol(v);
    return component(type, nodes, value);
}

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
