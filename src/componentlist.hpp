#ifndef COMPONENTLIST_HPP
#define COMPONENTLIST_HPP

#include <vector>
#include <set>
#include <algorithm>

#include "component.hpp"

class componentlist
{
    public:
        void add_component(const component& c)
        {
            components.push_back(c);
        }

        unsigned int number_of_nodes() const
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
            return all_nodes.size();
        }

        unsigned int number_of_devices(component_types type) const
        {
            return std::count_if(components.begin(), components.end(), [type] (const component& c) { return c.get_type() & type; });
        }

        unsigned int network_size() const
        {
            unsigned int num = 0;
            std::for_each(components.begin(), components.end(), [&num] (const component& c) { num += c.element_size(); });
            return number_of_nodes() + num;
        }

        std::vector<component>::const_iterator begin() const
        {
            return components.begin();
        }

        std::vector<component>::const_iterator end() const
        {
            return components.end();
        }

    private:
        std::vector<component> components;
};

#endif //COMPONENTLIST_HPP
