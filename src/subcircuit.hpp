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

    private:
        std::string name;
        std::vector<std::string> terminals;
        std::vector<component> components;
};

subcircuit read_subcircuit_header(const std::string& line);

#endif // SUBCKT_HPP
