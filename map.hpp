#ifndef MAP_HPP
#define MAP_HPP


#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <memory>
#include <map>
#include <ginac/ginac.h>

#include "stamp.hpp"

class map
{   
    public:
    map();
    map(std::map<std::string, unsigned int> mappy);
    void add_to_map (unsigned int, const std::string&, std::map< std::string, unsigned int> );       

    unsigned int get_max_node();
    std::vector<std::string> get_user_nodes();
    std::vector<unsigned int> get_calc_nodes();
    GiNaC::ex& get_map_value();
    
    
    private:
    std::vector<std::string> input_nodes;
    std::vector<unsigned int> output_nodes;
    unsigned int node;
    GiNaC::ex value;
};


#endif
