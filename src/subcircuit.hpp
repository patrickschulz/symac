#ifndef SUBCKT_HPP
#define SUBCKT_HPP

#include <string>
#include <vector>

#include "component.hpp"

class subcircuit
{
    public:
        subcircuit(const std::string& name = "", const std::vector<std::string>& terminals = {});

        std::string get_name() const;

        void add_component(const component& c);

        std::vector<component> get_mapped_components(const std::vector<std::string>& nodes);

    private:
        std::string name;
        std::vector<std::string> terminals;
        std::vector<component> components;
        unsigned int instance_number;
};

subcircuit read_subcircuit_header(const std::string& line);

#endif // SUBCKT_HPP
