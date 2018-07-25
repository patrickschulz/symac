#ifndef MAP_HPP
#define MAP_HPP



#include <string>
#include <map>


class map
{   
    public:
        map();
        unsigned int get_map_node(std::string snode);
        unsigned int get_number_nodes() const;
        unsigned int find_node(std::string snode) const;
        
    private:
        std::map<std::string, unsigned int> mappy;
};

#endif
