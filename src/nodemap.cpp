#include "nodemap.hpp"

#include <algorithm>
#include <iostream>

nodemap::nodemap() :
    internal_map({ { "0", 0 }, { "GND", 0 }, { "gnd", 0 } }),
    reverse_map({ { 0, "gnd" } })
{ 
}

unsigned int nodemap::operator[](const std::string& snode)
{ 
    std::map<std::string, unsigned int>::iterator it = internal_map.find(snode);
    if(it != internal_map.end())
    {
        return it->second;
    }
    else
    {
        unsigned int maxnode = get_number_nodes() + 1;
        internal_map.insert(std::make_pair(snode, maxnode));
        reverse_map.insert(std::make_pair(maxnode, snode));
        return maxnode;
    } 
}

std::vector<unsigned int> nodemap::operator[](const std::vector<std::string>& snodes)
{ 
    std::vector<unsigned int> mapped_nodes;
    for(const std::string& snode : snodes)
    {
        mapped_nodes.push_back(operator[](snode));
    }
    return mapped_nodes;
}

unsigned int nodemap::operator[](const std::string& snode) const
{ 
    std::map<std::string, unsigned int>::const_iterator it = internal_map.find(snode);
    if(it != internal_map.end())
    {
        return it->second;
    }
    else
    {
        return 0;
    } 
}

std::string nodemap::operator[](unsigned int inode) const
{
    auto it = reverse_map.find(inode + 1);
    if(it != reverse_map.end())
    {
        return it->second;
    }
    else
    {
        return "";
    } 
}

unsigned int nodemap::get_number_nodes() const
{
    typedef std::pair<std::string, unsigned int> pair_type;

    auto it = std::max_element
    (
         std::begin(internal_map), std::end(internal_map),
         [] (const pair_type & p1, const pair_type & p2) {
             return p1.second < p2.second;
         }
    );
    return it->second;
}

bool nodemap::check_node(const std::string& snode) const
{
     auto it = internal_map.find(snode);
     return it != internal_map.end();
}

