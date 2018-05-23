#include "map.hpp"


#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "netlist.hpp"

void map::add_to_map(unsigned int numbr_terminals, const std::string& line, std::map< std::string, unsigned int> my_map)
{
    std::string v;
    unsigned int node= 0;
    std::vector<unsigned int> output_nodes(numbr_terminals);
    std::vector<std::string> input_nodes(numbr_terminals);
    GiNaC::ex value;
    std::istringstream stream(line);
    stream >> v;
    std::map< std::string, unsigned int>::iterator it =my_map.begin(); 
    for (unsigned int i=0; i<numbr_terminals; ++i)
    {
        stream >> v;
        input_nodes.push_back(v);
        it = my_map.find(v);
        if(it != my_map.end())
        {
            output_nodes.push_back(it -> second);
        }
        else if((v == "0" || v == "GND") && it == my_map.end())
        {
            my_map.insert(std::make_pair(v,0)).first->second;
            output_nodes.push_back(0);
            input_nodes.push_back("0");
        }    
        else
        {
            it = my_map.end();
            --it;
            node = it -> second;
            node++;
            my_map.insert(std::make_pair(v,node)).first->second;
            output_nodes.push_back(node);
            input_nodes.push_back(v);
            
        }    
    }
    if(v.size() > 0 && v.find_first_not_of("0123456789.-") == std::string::npos) // is the string a numeric?
            {
                value = std::stod(v);
            }
            else
            {
                value = get_symbol(v);
            }
}
std::vector<std::string> map::get_user_nodes(){return input_nodes;}
std::vector<unsigned int> map::get_calc_nodes(){return output_nodes;}
GiNaC::ex& map::get_map_value(){ return value;}
unsigned int map::get_max_node()
{
    auto result = std::minmax_element(output_nodes.begin(), output_nodes.end());
    return output_nodes[result.second - output_nodes.begin()];
}








