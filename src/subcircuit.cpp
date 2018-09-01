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

std::vector<component> subcircuit::get_mapped_components(const std::vector<std::string>& nodes)
{
    std::map<std::string, std::string> submap;
    for(unsigned int i = 0; i < terminals.size(); ++i)
    {
        submap[terminals[i]] = nodes[i];
    }

    std::vector<component> mapped_components;
    for(component c : components)
    {
        std::vector<std::string> mapped_nodes;
        for(const std::string& node : c.get_nodes())
        {
            std::string mapped_node = node;
            if(node.find("!") == std::string::npos) // node is not global
            {
                auto it = submap.find(node);
                if(it != submap.end())
                {
                    mapped_node = submap[node];
                }
                else // local net
                {
                    // TODO
                    //mapped_node = node + ":" + std::to_string(nlist.subinstances[subname]);
                    mapped_node = node + ":" + std::to_string(1);
                }
            }
            mapped_nodes.push_back(mapped_node);
        }
        c.set_nodes(mapped_nodes);
        mapped_components.push_back(c);
    }
    return mapped_components;
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

