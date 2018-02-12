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
    ct_resistor = 1,
    ct_capacitor = 2,
    ct_inductor = 4,
    ct_voltage_source = 8,
    ct_current_source = 16,
    ct_opamp = 32,
    ct_voltage_controlled_voltage_source = 64,
    ct_current_controlled_voltage_source = 128,
    ct_voltage_controlled_current_source = 256,
    ct_current_controlled_current_source = 512
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
        component() { }
        component(GiNaC::ex value) :
            value(value)
        {

        }
        virtual ~component() { }

        virtual component_types type() const = 0;
        virtual char short_type() const = 0;
        virtual std::string to_string() const = 0;
        virtual std::vector<unsigned int> get_nodes() const = 0;
        virtual unsigned int get_max_node() const = 0;
        void reset_stamp();
        virtual void set_stamp(netlist&) = 0;
        const std::vector<stamp>& get_stamps() const
        {
            return stamps;
        }

        const GiNaC::ex& get_value() const { return value; }

        virtual /* TODO: magic type */ void get_stamp() const = 0;
    protected:
        GiNaC::ex value;
        std::vector<stamp> stamps;
};

class two_terminal_device : public component
{
    public:
        explicit two_terminal_device(const std::string& line)
        {
            std::string v;
            std::istringstream stream(line);
            char type;
            stream >> type >> node1 >> node2 >> v;
            value = get_symbol(v);
        }

        explicit two_terminal_device(unsigned int node1, unsigned int node2, GiNaC::ex value) :
            component(value), node1(node1), node2(node2)
        {

        }

        explicit two_terminal_device(unsigned int node1, unsigned int node2, const std::string& value) :
            component(get_symbol(value)), node1(node1), node2(node2)
        {

        }

        virtual std::string to_string() const override
        {
            std::ostringstream stream;
            stream << component_names[type()] << ':' << node1 << '-' << node2 << " (" << value << ")";
            return stream.str();
        }
        
        virtual std::vector<unsigned int> get_nodes() const
        {
            return { node1, node2 };
        }
         
        virtual unsigned int get_max_node() const
        {
            return std::max(node1, node2);
        }

        virtual /* TODO: magic type */ void get_stamp() const override
        {

        }
    protected:
        unsigned int node1;
        unsigned int node2;
};

class three_terminal_device : public component
{
    public:
        explicit three_terminal_device(const std::string& line)
        {
            std::string v;
            std::istringstream stream(line);
            char type;
            stream >> type >> node1 >> node2 >> node3 >> v;
            value = get_symbol(v);
        }

        virtual std::string to_string() const override
        {
            std::ostringstream stream;
            stream << component_names[type()] << ':' << node1 << '-' << node2 << ':' << node3 << " (" << value << ")";
            return stream.str();
        }

        virtual std::vector<unsigned int> get_nodes() const
        {
            return { node1, node2, node3 };
        }

        virtual unsigned int get_max_node() const
        {
            return std::max(std::max(node1, node2), node3);
        }

        virtual /* TODO: magic type */ void get_stamp() const override
        {

        }
    protected:
        unsigned int node1;
        unsigned int node2;
        unsigned int node3;
};

class four_terminal_device : public component
{
    public:
        explicit four_terminal_device(const std::string& line)
        {
            std::string v;
            std::istringstream stream(line);
            char type;
            stream >> type >> node1 >> node2 >> node3 >> node4 >> v;
            value = get_symbol(v);
        }

        virtual std::string to_string() const override
        {
            std::ostringstream stream;
            stream << component_names[type()] << ':' << node1 << '-' << node2 << ' ' << node3 << '-' << node4 << " (" << value << ")";
            return stream.str();
        }

        virtual std::vector<unsigned int> get_nodes() const
        {
            return { node1, node2, node3, node4 };
        }

        virtual unsigned int get_max_node() const
        {
            return std::max(std::max(node1, node2), std::max(node3, node4));
        }

        virtual /* TODO: magic type */ void get_stamp() const override
        {

        }
    protected:
        unsigned int node1;
        unsigned int node2;
        unsigned int node3;
        unsigned int node4;
};

class resistor : public two_terminal_device
{
    public:
        using two_terminal_device::two_terminal_device;

        virtual void set_stamp(netlist&);

        virtual component_types type() const override
        {
            return component_types::ct_resistor;
        }

        virtual char short_type() const override
        {
            return 'R';
        }
};

class capacitor : public two_terminal_device
{
    public:
        using two_terminal_device::two_terminal_device;

        virtual void set_stamp(netlist&);

        virtual component_types type() const override
        {
            return component_types::ct_capacitor;
        }

        virtual char short_type() const override
        {
            return 'C';
        }
};

class inductor : public two_terminal_device
{
    public:
        using two_terminal_device::two_terminal_device;

        virtual void set_stamp(netlist&);

        virtual component_types type() const override
        {
            return component_types::ct_inductor;
        }

        virtual char short_type() const override
        {
            return 'L';
        }
};

class voltage_source : public two_terminal_device
{
    public:
        using two_terminal_device::two_terminal_device;

        virtual void set_stamp(netlist&);

        virtual component_types type() const override
        {
            return component_types::ct_voltage_source;
        }

        virtual char short_type() const override
        {
            return 'V';
        }
};

class current_source : public two_terminal_device
{
    public:
        using two_terminal_device::two_terminal_device;

        virtual void set_stamp(netlist&);

        virtual component_types type() const override
        {
            return component_types::ct_current_source;
        }

        virtual char short_type() const override
        {
            return 'I';
        }
};

class opamp : public three_terminal_device
{
    public:
        using three_terminal_device::three_terminal_device;

        virtual void set_stamp(netlist&);

        virtual component_types type() const override
        {
            return component_types::ct_opamp;
        }

        virtual char short_type() const override
        {
            return 'O';
        }
};

class voltage_controlled_voltage_source : public four_terminal_device
{
    public:
        using four_terminal_device::four_terminal_device;

        virtual void set_stamp(netlist&);

        virtual component_types type() const override
        {
            return component_types::ct_voltage_controlled_voltage_source;
        }

        virtual char short_type() const override
        {
            return 'E';
        }
};

class current_controlled_voltage_source : public four_terminal_device
{
    public:
        using four_terminal_device::four_terminal_device;

        virtual void set_stamp(netlist&);

        virtual component_types type() const override
        {
            return component_types::ct_current_controlled_voltage_source;
        }

        virtual char short_type() const override
        {
            return 'F';
        }
};

class voltage_controlled_current_source : public four_terminal_device
{
    public:
        using four_terminal_device::four_terminal_device;

        virtual void set_stamp(netlist&);

        virtual component_types type() const override
        {
            return component_types::ct_voltage_controlled_current_source;
        }

        virtual char short_type() const override
        {
            return 'G';
        }
};

class current_controlled_current_source : public four_terminal_device
{
    public:
        using four_terminal_device::four_terminal_device;

        virtual void set_stamp(netlist&);

        virtual component_types type() const override
        {
            return component_types::ct_current_controlled_current_source;
        }

        virtual char short_type() const override
        {
            return 'G';
        }
};

std::unique_ptr<component> create_component(const std::string& line);

#endif //COMPONENT_HPP
