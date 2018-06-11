#ifndef SUBCKT_HPP
#define SUBCKT_HPP
#include <string>
#include <vector>

class subcircuit
{
    public:
        subcircuit(std::string name, std::vector<std::string> terminals):
        subckt_name(name), terminals(terminals)
        { };    
        void clear();
        std::string get_name(); 
        std::vector<std::string> get_sublines();
        void add_line(const std::string &line);
        std::vector<std::string> get_terminals();
    private:
        std::string subckt_name;
        std::vector<std::string> terminals;
        std::vector<std::string> sub_lines;
};


#endif
