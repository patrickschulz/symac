#ifndef COMPONENTLIST_HPP
#define COMPONENTLIST_HPP

#include <vector>
#include <string>

#include "component.hpp"

class componentlist
{
    public:
        void add_component(const component& c);

        std::vector<std::string> get_nodes() const;

        unsigned int number_of_nodes() const;
        unsigned int number_of_devices(component_types type) const;
        unsigned int network_size() const;

        std::vector<component>::const_iterator begin() const;
        std::vector<component>::const_iterator end() const;

    private:
        std::vector<component> components;
};

#endif //COMPONENTLIST_HPP
