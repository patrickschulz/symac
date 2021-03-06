#ifndef COMPONENTLIST_HPP
#define COMPONENTLIST_HPP

#include <vector>
#include <string>

#include "component.hpp"

class componentlist
{
    public:
        void add(const component& c);
        void add(const std::vector<component>& cv);

        std::vector<component> get_components_by_type(component_types ct) const;
        std::vector<std::string> get_nodes() const;

        unsigned int number_of_components(component_types ct) const;

        unsigned int number_of_nodes() const;

        std::vector<component>::const_iterator begin() const;
        std::vector<component>::const_iterator end() const;

    private:
        std::vector<component> components;
};

#endif //COMPONENTLIST_HPP
