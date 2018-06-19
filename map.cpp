#include "map.hpp"

#include <vector>
#include <sstream>

#include "netlist.hpp"

map::map()
{ 
    mappy["0"]=0;
}
void map::add_to_map(std::string snode)
{
    
    unsigned int inode;
    std::map<std::string, unsigned int>::iterator it =mappy.begin(); 
    
        
        it = mappy.find(snode);
        if(it != mappy.end())
        {
            mapped_node = it -> second;
        }
        else if(snode == "0" ||snode  == "GND" || snode == "gnd" || snode == "Gnd")
        {
            mapped_node = 0;
        }    
        else
        {
            it = mappy.end();
            --it;
            inode = it -> second;
            inode++;
            mappy.insert(std::make_pair(snode,inode)).first->second;
            mapped_node = inode;
        } 
}    
unsigned int map::get_map_node(){ return mapped_node;}
unsigned int map::get_number_nodes() const
{
    unsigned int i = mappy.size();
    i--;
    return i;
}
unsigned int map::find_node(std::string snode) const
{
     unsigned int unode;

     auto it = mappy.find(snode);
     if (it != mappy.end())
     {
         unode = it-> second;
     }
     else
     {
         unode = 0;
     }
     return unode;
}
            
         
         
         
         
         
         
         
         
         
         
         
         
// TODO : put funct in netlist
// unsigned int map::get_max_node()
// {
//     auto result = std::minmax_element(output_nodes.begin(), output_nodes.end());
//     return output_nodes[result.second - output_nodes.begin()];
// }

// std::string map::get_user_node(unsigned int unode)
// {
//     std::map<unsigned int, std::string > reverse;
//     std::string usernode = "Error: Could not find";
//     std::map<std::string, unsigned int>::iterator i;
//     std::map<unsigned int,std::string>::iterator h;
//     for ( i = mappy.begin(); i != mappy.end(); ++i)
//     {
//         reverse[i->second] = i->first;
//     }
//     h = reverse.find(unode);
//     if(h != reverse.end())
//     {
//         usernode= i -> second;
//     }
//     return usernode;
// }
