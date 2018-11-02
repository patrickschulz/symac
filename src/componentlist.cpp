#include "componentlist.hpp"

#include <set>
#include <algorithm>

void componentlist::add_component(const component& c)
{
    components.push_back(c);
}

void componentlist::add_component(const std::vector<component>& cv)
{
    for(const auto& c : cv)
    {
        add_component(c);
    }
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

unsigned int componentlist::number_of_nodes() const
{
    const auto& nodes = get_nodes();
    return nodes.size();
}

unsigned int componentlist::number_of_devices(component_types type) const
{
    return std::count_if(components.begin(), components.end(), [type] (const component& c) { return c.get_type() & type; });
}

unsigned int componentlist::network_size() const
{
    unsigned int num = 0;
    std::for_each(components.begin(), components.end(), [&num] (const component& c) { num += c.element_size(); });
    return number_of_nodes() + num;
}

std::vector<component>::const_iterator componentlist::begin() const
{
    return components.begin();
}

std::vector<component>::const_iterator componentlist::end() const
{
    return components.end();
}

