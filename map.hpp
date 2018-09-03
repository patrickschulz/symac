#ifndef MAP_HPP
#define MAP_HPP



#include <string>
#include <map>


class map
{   
    public:
        map();
        // Mapping for calculating
        unsigned int get_map_node(std::string snode);
        unsigned int get_number_nodes() const;
        unsigned int find_node(std::string snode) const;
        //Mapping for output
        std::string  get_output_node(unsigned int inode);
        void add_to_output_map(unsigned int inode, std::string unode);
        
    private:
        std::map<std::string, unsigned int> mappy;
        //Map for output;
        std::map<unsigned int, std::string> rev_mappy;
};

#endif
