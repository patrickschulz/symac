#include "netlist.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <ginac/ginac.h>

#include "util.hpp"
#include "symbol.hpp"
#include "parser/netlist_parser.hpp"

unsigned int netlist::number_terminals(char type)
{
    unsigned int num = 0;
    switch(type)
    {
        case 'R': // resistor
        case 'C': // capacitor
        case 'L': // inductor
        case 'V': // voltage_source
        case 'I': // current_source
            num = 2;
            break;
        case 'O': // opamp
            num = 3;
            break;
        case 'E': // voltage controlled voltage source
        case 'F': // current controlled voltage source
        case 'G': // voltage_controlled_current_source
        case 'H': // current_controlled_current_source
            num = 4;
            break;
        default:
            std::cerr << "Unknown component: '" << type << "'\n";
            return 0;
    }
    return num;
}

netlist::netlist() :
    valid(false)
{
}

netlist::netlist(const std::string& filename) :
    netlist()
{
    read(filename);
}

void netlist::add_component(const component& c)
{
    components.add_component(c);
}

struct netlist_printer_type : public boost::static_visitor<>
{
    public:
        void operator() (const component& c) const
        {
            std::cout << "component: " << c << '\n';
        }

        void operator() (const std::string& str) const
        {
            std::cout << '"' << str << '"' << '\n';
        }
} netlist_printer;

void netlist::read(std::string filename)
{
    std::ifstream file(filename);
    if(!file.is_open())
    {
        valid = false;
        //return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string str = buffer.str();
    auto iter = str.begin();
    std::vector<boost::variant<component, std::string>> lines;
    bool r = qi::phrase_parse(iter, str.end(), netlist_parser, qi::ascii::blank, lines);

    if (r && iter == str.end())
    {
        std::cout << "parse succeeded\n";
        for(auto e : lines)
        {
            apply_visitor(netlist_printer, e);
        }
    }
    else
    {
        std::cout << "parse failed. Remaining unparsed: " << std::string(iter, str.end()) << '\n';
    }

    valid = true;
}

netlist::operator bool()
{
    return valid;
}

static GiNaC::ex check_and_convert_numeric_symbol(const std::string& v)
{
    GiNaC::ex value;
    if(v.size() > 0 && v.find_first_not_of("0123456789.-") == std::string::npos) // is the string a numeric?
    {
        value = std::stod(v);
    }
    else
    {
        value = get_symbol(v);
    }
    return value;
}
