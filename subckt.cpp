#include "subckt.hpp"

#include <sstream>
#include <iostream>
#include <vector>
#include <string>

// Annahme:
// 1) nodes in subckt stimmen nicht denen im circuit überein
// 2) values werden nicht doppelt verwendet

void subcircuit::read_subckt(const std::string& line)
{
    valid = false;
    char dot;
    std::string v;
    std::string n = get_name();
    std::istringstream stream(line);
    stream >> dot;
    stream >> v;
    if (v=="subckt")
    { read_header(line);}
    else if (v=="end")
    { end_subckt();}
    else if (v=="[A-Z]")
    { read_body(line);}
    else if (v==n)
    { read_desc(line);}
}
void subcircuit::read_header(const std::string& line)
{
    char x;
    std::string v;
    unsigned int number_terminals;
    std::vector<std::string> terminal_names;
    std::istringstream stream(line);
    
    stream >> x;
    stream >> v; 
    set_name(v);
    stream >> number_terminals;
    set_nmbr_terminals(number_terminals);
    for(unsigned int i = 0; i<number_terminals; ++i)
    {
        stream >> v ;
        terminal_names.push_back(v);
    }
    set_terminals(terminal_names);
}
void subcircuit::end_subckt()
{
    valid = true;
}    
void subcircuit::read_body(const std::string& line)
{

    std::string u = line;
    u.erase(u.begin());
    set_subline(u);
}
void subcircuit::read_desc(const std::string& line)
{
    std::string q = line;
    q.erase(q.begin());
    std::string u;
    std::string t;
    
    std::vector<std::string> v    = get_terminals();
    std::vector<std::string> c    = get_subline();
    unsigned int number_terminals = get_nmbr_terminals();
    std::string name              = get_name();
    std::stringstream stream(q);
    
    stream >> u;
    for(unsigned int i=0; i < number_terminals; i++)
    {           
        stream >> u;
        t = v.at(i);
        for( unsigned int j = 0; j < number_terminals; j++)
        {
            q = c.at(j);
            std::size_t find = q.find(t);
            if(find != std::string::npos)
            {
                q.erase(q.begin()+find);
                q.insert(find,u);
//                 q.erase(q.begin()+find-1);
            }
        }
        set_subline(q);
    }
    
}
void subcircuit::set_name(const std::string& muh) 
{
    name = muh;
}
std::string subcircuit::get_name(){return name;}
void subcircuit::set_nmbr_terminals(unsigned int number)
{
    nmbr_terminals = number;
}
unsigned int subcircuit::get_nmbr_terminals(){ return nmbr_terminals;}
void subcircuit::set_terminals(std::vector<std::string> term)
{
    terminals = term;
}
std::vector<std::string>subcircuit::get_terminals(){return terminals;}
void subcircuit::set_subline(const std::string& line)
{
    sub_lines.push_back(line);
}
std::vector<std::string>subcircuit::get_subline(){return sub_lines;}
//bool for netlist_funct to insert into line
void subcircuit::set_bool()
{
    valid = true;
}
bool subcircuit::get_bool(){return valid;}






