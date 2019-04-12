#include "subcircuit.hpp"

std::vector<component> get_mapped_components(const subcircuit& sub, const std::string& instance, const std::vector<std::string>& nodes)
{
    // map the subcircuit terminals to the instance terminals
    std::map<std::string, std::string> submap;
    for(unsigned int i = 0; i < sub.terminals.size(); ++i)
    {
        submap[sub.terminals[i]] = nodes[i];
    }

    std::vector<component> mapped_components;
    for(component c : sub.components)
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
                    mapped_node = instance + ":" + node;
                }
            }
            mapped_nodes.push_back(mapped_node);
        }
        c.set_nodes(mapped_nodes);
        mapped_components.push_back(c);
    }
    return mapped_components;
}

