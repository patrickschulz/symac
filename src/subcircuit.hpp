#ifndef SUBCIRCUIT_HPP
#define SUBCIRCUIT_HPP

#include <string>
#include <vector>

#include "component.hpp"

struct subcircuit
{
    std::string name;
    std::vector<std::string> terminals;
    std::vector<component> components;
};

struct subcircuit_instance
{
    std::string instance;
    std::string name;
    std::vector<std::string> terminals;
};

std::vector<component> get_mapped_components(const subcircuit& sub, const std::string& instance, const std::vector<std::string>& nodes);


#endif // SUBCIRCUIT_HPP
