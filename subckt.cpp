#include "subckt.hpp"

#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>







std::string subcircuit::get_name(){return subckt_name;}
std::vector<std::string>subcircuit::get_sublines(){return sub_lines;}
void subcircuit::add_line(const std::string& line)
{
    sub_lines.push_back(std::move(line));
}
std::vector<std::string> subcircuit::get_terminals()
{return terminals;}
void subcircuit::clear()
{ sub_lines.clear();}









