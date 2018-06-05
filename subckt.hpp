#ifndef SUBCKT_HPP
#define SUBCKT_HPP
#include <string>
#include <vector>

class subcircuit
{
    public:
        subcircuit(std::string name, std::vector<std::string> terminals):
        name(name), terminals(terminals)
        { };    

        std::string get_name(); 
        std::vector<std::string> get_sublines();
        void add_line(const std::string &line);
        void change_lines();
        unsigned int numbr_terminals(char type);
        std::vector<std::string> get_terminals();
    private:
        bool valid;
        std::string name;
        std::vector<std::string> terminals;
        std::vector<std::string> sub_lines;
};


#endif
