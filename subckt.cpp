#include "subckt.hpp"

#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>


std::string subcircuit::get_name(){return name;}
std::vector<std::string>subcircuit::get_sublines(){return sub_lines;}
void subcircuit::add_line(const std::string& line)
{
    sub_lines.push_back(line);
}
void subcircuit::change_lines()
{
    for (unsigned int i = 0; i < sub_lines.size();i++)
    {
        std::string sline = sub_lines[i];
        std::stringstream stream(sline);
        char type;
        std::string port;
        unsigned int nterminals;
        stream >> type;
        nterminals = numbr_terminals(type);
        stream >> port;
        for (unsigned int i = 0 ; i< nterminals; i++)
        {   
            if(std::find(terminals.begin(),terminals.end(), port) != terminals.end())
            {
                stream >> port;
            }
            else
            {
                unsigned int unode = 42 * (i+10);
                stream << unode;
                stream >> port;
            }
    
        }
        sub_lines[i]= sline;
    }
}
unsigned int subcircuit::numbr_terminals(char type)
{
    unsigned int number_terminals;
    if ( type == ( 'V' ||'R'||'I'||'L'||'C'))
    {
        number_terminals = 2;
        
    }
    if (type == 'O')
    {
        number_terminals = 3;
        
    }
    if (type == ('E'||'F'||'G'||'H'))
    {
        number_terminals = 4;
        
    }
    return number_terminals;
}
std::vector<std::string> subcircuit::get_terminals()
{return terminals;}










