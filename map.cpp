#include "map.hpp"

#include <algorithm>

map::map()
{ 
    mappy["0"] = 0;
}

unsigned int map::get_mapped_node(const std::string& snode)
{ 
    std::map<std::string, unsigned int>::iterator it = mappy.find(snode);
    if(it != mappy.end())
    {
        return it->second;
    }
    else if(snode == "0" ||snode  == "GND" || snode == "gnd" || snode == "Gnd")
    {
        return 0;
    }    
    else
    {
        unsigned int maxnode = get_number_nodes() + 1;
        mappy.insert(std::make_pair(snode, maxnode));
        return maxnode;
    } 
}

unsigned int map::get_number_nodes() const
{
    typedef std::pair<std::string, unsigned int> pair_type;

    auto it = std::max_element
    (
         std::begin(mappy), std::end(mappy),
         [] (const pair_type & p1, const pair_type & p2) {
             return p1.second < p2.second;
         }
    );
    return it->second;
}
