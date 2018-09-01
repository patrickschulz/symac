#include "subcircuit.hpp"

#include <sstream>

subcircuit::subcircuit(const std::string& name, const std::vector<std::string>& terminals) :
    name(name), terminals(terminals)
{

}

std::string subcircuit::get_name() const
{
    return name;
}

void subcircuit::add_component(const component& c)
{
    components.push_back(c);
}

subcircuit read_subcircuit_header(const std::string& line)
{
    std::istringstream stream(line);
    std::string name;
    std::string temp;
    std::vector<std::string> terminals;
    stream >> temp >> name; // skip ".SUBCKT" 
    while(stream >> temp)
    {
        terminals.push_back(temp);
    }
    return subcircuit(name, terminals);
}

