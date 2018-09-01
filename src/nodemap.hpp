#ifndef MAP_HPP
#define MAP_HPP

#include <string>
#include <map>
#include <vector>

class nodemap
{   
    public:
        nodemap();
        unsigned int operator[](const std::string& snode);
        std::vector<unsigned int> operator[](const std::vector<std::string>& snodes);
        unsigned int operator[](const std::string& snode) const;
        std::string operator[](unsigned int inode) const;
        unsigned int get_number_nodes() const;
        bool check_node(const std::string& snode) const;
        
    private:
        std::map<std::string, unsigned int> internal_map;
        std::map<unsigned int, std::string> reverse_map;
};

#endif
