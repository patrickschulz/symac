#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <memory>
#include <ginac/ginac.h>

#include "stamp.hpp"

class netlist;

enum component_types
{
    ct_none                              = 0,
    ct_resistor                          = 1 << 0,
    ct_capacitor                         = 1 << 1,
    ct_inductor                          = 1 << 2,
    ct_voltage_source                    = 1 << 3,
    ct_current_source                    = 1 << 4,
    ct_opamp                             = 1 << 5,
    ct_voltage_controlled_voltage_source = 1 << 6,
    ct_current_controlled_voltage_source = 1 << 7,
    ct_voltage_controlled_current_source = 1 << 8,
    ct_current_controlled_current_source = 1 << 9
};
inline component_types operator|(component_types a, component_types b)
{
    return static_cast<component_types>(static_cast<int>(a) | static_cast<int>(b));
}

const char component_names[] = {
    'R', // ct_resistor
    'C', // ct_capacitor
    'L', // ct_inductor
    'V', // ct_voltage_source
    'I', // ct_current_source
    'O', // ct_opamp
    'E', // ct_voltage_controlled_voltage_source
    'F', // ct_current_controlled_voltage_source
    'G', // ct_voltage_controlled_current_source
    'H', // ct_current_controlled_current_source
};

class component
{
    public:
        component(unsigned int number_of_terminals, const std::string& line) :
            nodes(number_of_terminals, 0)
        {
            std::istringstream stream(line);

            char type;
            stream >> type;
            for(unsigned int i = 0; i < nodes.size(); ++i)
            {
                unsigned int node;
                stream >> node;
                nodes[i] = node;
            }
            std::string v;
            stream >> v;
            if(v.size() > 0 && v.find_first_not_of("0123456789.-") == std::string::npos) // is the string a numeric?
            {
                value = std::stod(v);
            }
            else
            {
                value = get_symbol(v);
            }
        }
        virtual ~component() { }

        virtual component_types type() const = 0;

        virtual std::string to_string() const
        {
            std::ostringstream stream;
            stream << component_names[type()];
            for(unsigned int i = 0; i < nodes.size(); ++i)
            {
                stream << ':' << nodes[i] << '-';
            }
            stream << nodes[nodes.size() - 1];
            stream << " (" << value << ")";
            return stream.str();
        }

        const std::vector<unsigned int>& get_nodes() const
        {
            return nodes;
        }

        virtual unsigned int get_max_node() const
        {
            auto result = std::minmax_element(nodes.begin(), nodes.end());
            return nodes[result.second - nodes.begin()];
        }

        void reset_stamp()
        {
            stmp.clear();
        }

        virtual void set_stamp(netlist&) = 0;

        const stamp& get_stamp() const
        {
            return stmp;
        }

        const GiNaC::ex& get_value() const { return value; }
    protected:
        std::vector<unsigned int> nodes;
        GiNaC::ex value;
        stamp stmp;
};

class two_terminal_device : public component
{
    public:
        explicit two_terminal_device(const std::string& line) :
            component(2, line)
        {
        }
};

class three_terminal_device : public component
{
    public:
        explicit three_terminal_device(const std::string& line) :
            component(3, line)
        {
        }
};

class four_terminal_device : public component
{
    public:
        explicit four_terminal_device(const std::string& line) :
            component(4, line)
        {
        }
};

class impedance : public two_terminal_device
{
    public:
        using two_terminal_device::two_terminal_device;

        virtual void set_stamp(netlist&) override;
};

class resistor : public impedance
{
    public:
        resistor(const std::string& line) :
            impedance(line)
        {
            value = 1/value; // convert the raw value to an conductance
        }

        virtual component_types type() const override
        {
            return component_types::ct_resistor;
        }
};

class capacitor : public impedance
{
    public:
        capacitor(const std::string& line) :
            impedance(line)
        {
            GiNaC::ex s = get_symbol("s");
            value = s * value; // convert the raw value to an conductance
        }

        virtual component_types type() const override
        {
            return component_types::ct_capacitor;
        }
};

class inductor : public impedance
{
    public:
        inductor(const std::string& line) :
            impedance(line)
        {
            GiNaC::ex s = get_symbol("s");
            value = 1 / (s * value); // convert the raw value to an conductance
        }

        virtual component_types type() const override
        {
            return component_types::ct_inductor;
        }
};

class voltage_source : public two_terminal_device
{
    public:
        using two_terminal_device::two_terminal_device;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_voltage_source;
        }
};

class current_source : public two_terminal_device
{
    public:
        using two_terminal_device::two_terminal_device;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_current_source;
        }
};

class opamp : public three_terminal_device
{
    public:
        using three_terminal_device::three_terminal_device;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_opamp;
        }
};

class voltage_controlled_voltage_source : public four_terminal_device
{
    public:
        using four_terminal_device::four_terminal_device;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_voltage_controlled_voltage_source;
        }
};

class current_controlled_voltage_source : public four_terminal_device
{
    public:
        using four_terminal_device::four_terminal_device;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_current_controlled_voltage_source;
        }
};

class voltage_controlled_current_source : public four_terminal_device
{
    public:
        using four_terminal_device::four_terminal_device;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_voltage_controlled_current_source;
        }
};

class current_controlled_current_source : public four_terminal_device
{
    public:
        using four_terminal_device::four_terminal_device;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_current_controlled_current_source;
        }
};

std::unique_ptr<component> create_component(const std::string& line);

#endif //COMPONENT_HPP
