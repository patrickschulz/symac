#ifndef MAP_HPP
#define MAP_HPP

#include <string>
#include <map>

class map
{   
    public:
        map();
        unsigned int get_mapped_node(const std::string&);
        unsigned int get_number_nodes() const;
        
    private:
        std::map<std::string, unsigned int> mappy;
};

#endif
