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
#include "map.hpp"

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
        component(const std::vector<unsigned int>& nodes, const GiNaC::ex& value) :
            nodes(nodes), value(value)
        {   }
        
        virtual component_types type() const = 0;
        virtual char short_type() const = 0;

        const std::vector<unsigned int>& get_nodes() const
        {
            return nodes;
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

        const GiNaC::ex& get_value() const
        {
            return value;
        }

    protected:
        std::vector<unsigned int> nodes;
        GiNaC::ex value;
        stamp stmp;
};

class impedance : public component
{
    public:
        using component::component;

        virtual void set_stamp(netlist& nlist) override;
};

class resistor : public impedance
{
    public:
        resistor(const std::vector<unsigned int>& nodes, const GiNaC::ex& value) :
            impedance(nodes, value)
        {   
            this->value = 1 / value;
        }

        virtual component_types type() const override
        {
            return component_types::ct_resistor;
        }

        virtual char short_type() const override
        {
            return 'R';
        }
};

class capacitor : public impedance
{
    public:
        capacitor(const std::vector<unsigned int>& nodes, const GiNaC::ex& value) :
            impedance(nodes, value)
        {   
            GiNaC::ex s = get_symbol("s");
            this->value = s * value;
        }

        virtual component_types type() const override
        {
            return component_types::ct_capacitor;
        }

        virtual char short_type() const override
        {
            return 'C';
        }
};

class inductor : public impedance
{
    public:
        inductor(const std::vector<unsigned int>& nodes, const GiNaC::ex& value) :
            impedance(nodes, value)
        {   
            GiNaC::ex s = get_symbol("s");
            this->value = 1 / (s * value);
        }

        virtual component_types type() const override
        {
            return component_types::ct_inductor;
        }

        virtual char short_type() const override
        {
            return 'L';
        }
};

class voltage_source : public component
{
    public:
        using component::component;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_voltage_source;
        }

        virtual char short_type() const override
        {
            return 'V';
        }
};

class current_source : public component
{
    public:
        using component::component;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_current_source;
        }

        virtual char short_type() const override
        {
            return 'I';
        }
};

class opamp : public component
{
    public:
        using component::component;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_opamp;
        }

        virtual char short_type() const override
        {
            return 'O';
        }
};

class voltage_controlled_voltage_source : public component
{
    public:
        using component::component;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_voltage_controlled_voltage_source;
        }

        virtual char short_type() const override
        {
            return 'E';
        }
};

class current_controlled_voltage_source : public component
{
    public:
        using component::component;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_current_controlled_voltage_source;
        }

        virtual char short_type() const override
        {
            return 'F';
        }
};

class voltage_controlled_current_source : public component
{
    public:
        using component::component;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_voltage_controlled_current_source;
        }

        virtual char short_type() const override
        {
            return 'G';
        }
};

class current_controlled_current_source : public component
{
    public:
        using component::component;

        virtual void set_stamp(netlist&) override;

        virtual component_types type() const override
        {
            return component_types::ct_current_controlled_current_source;
        }

        virtual char short_type() const override
        {
            return 'H';
        }
};

std::unique_ptr<component> create_component(char type, std::vector<unsigned int> nodes, GiNaC::ex& value );

#endif //COMPONENT_HPP
