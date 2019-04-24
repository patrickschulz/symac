#include "componentlist.hpp"

#include <set>
#include <algorithm>

void componentlist::add(const component& c)
{
    components.push_back(c);
}

void componentlist::add(const std::vector<component>& cv)
{
    for(const auto& c : cv)
    {
        add(c);
    }
}

std::vector<component> componentlist::get_components_by_type(component_types ct) const
{
    std::vector<component> subset;
    for(const component& c : components)
    {
        if(c.get_type() == ct)
        {
            subset.push_back(c);
        }
    }
    return subset;
}

std::vector<std::string> componentlist::get_nodes() const
{
    std::set<std::string> all_nodes;
    for(const component& c : components)
    {
        std::vector<std::string> nodes = c.get_nodes();
        for(const std::string& str : nodes)
        {
            if(str != "0")
            {
                all_nodes.insert(str);
            }
        }
    }
    return std::vector<std::string>(all_nodes.begin(), all_nodes.end());
}

unsigned int componentlist::number_of_components(component_types ct) const
{
    auto vec = get_components_by_type(ct);
    return vec.size();
}

unsigned int componentlist::number_of_nodes() const
{
    const auto& nodes = get_nodes();
    return nodes.size();
}

std::vector<component>::const_iterator componentlist::begin() const
{
    return components.begin();
}

std::vector<component>::const_iterator componentlist::end() const
{
    return components.end();
}

