#ifndef SUBCKT_HPP
#define SUBCKT_HPP
#include <string>
#include <vector>

class subcircuit
{
    public:
        subcircuit();
        
        void read_subckt(const std::string& line);
        void read_header(const std::string& line);
        void read_body(const std::string& line);
        void read_desc(const std::string& line);
        void end_subckt();
        std::string to_line();
        
        void set_name(const std::string& muh);
        std::string get_name();
        void set_nmbr_terminals(unsigned int number);
        unsigned int get_nmbr_terminals();
        void set_terminals(std::vector<std::string> term);
        std::vector<std::string> get_terminals();
        void set_subline(const std::string& line);
        std::vector<std::string> get_subline();
        void set_bool();
        bool get_bool();
        
    private:
        static bool valid;
        std::string name;
        unsigned int nmbr_terminals;
        std::vector<std::string> terminals;
        std::vector<std::string> sub_lines;
};


#endif
