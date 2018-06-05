#ifndef MAP_HPP
#define MAP_HPP



#include <string>
#include <map>


class map
{   
    public:
        map();
        void add_to_map(std::string snode);
        unsigned int get_map_node();
        unsigned int get_number_nodes() const;
        
    private:
        std::map<std::string, unsigned int> mappy;
        unsigned int mapped_node;
};

#endif
