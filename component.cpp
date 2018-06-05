#include "component.hpp"
#include "netlist.hpp"
#include <utility>
std::unique_ptr<component> create_component(char type, std::vector< unsigned int> nodes, GiNaC::ex& value)
{ 
    
    switch(type)
    {
        case 'R':
            return std::make_unique<resistor>(nodes,value);
        case 'C':
            return std::make_unique<capacitor>(nodes,value);
        case 'L':
            return std::make_unique<inductor>(nodes,value);
        case 'V':
            return std::make_unique<voltage_source>(nodes,value);
        case 'I':
            return std::make_unique<current_source>(nodes,value);
        case 'E':
            return std::make_unique<voltage_controlled_voltage_source>(nodes,value);
        case 'O':
            return std::make_unique<opamp>(nodes,value);
        case 'F':
            return std::make_unique<current_controlled_voltage_source>(nodes,value);
        case 'G':
            return std::make_unique<voltage_controlled_current_source>(nodes,value);
        case 'H':
            return std::make_unique<current_controlled_current_source>(nodes,value);
        default:
            std::cerr << "Unknown component: '" << type << "'\n";
            return NULL;
    }
}

void resistor::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.get_current_impedance();
    nlist.increment_current_impedance();
    if(nodes[0] > 0)
    {
        stamps.push_back(element(offset, nodes[0], 1/value));
        stamps.push_back(element(nodes[0], offset, 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(element(offset, nodes[1], -1/value));
        stamps.push_back(element(nodes[1], offset, -1));
    }
    stamps.push_back(element(offset, offset, -1));
}

void capacitor::set_stamp(netlist& nlist)
{
    GiNaC::ex s = get_symbol("s");
    unsigned int offset = nlist.number_of_nodes() + nlist.get_current_impedance();
    nlist.increment_current_impedance();
    if(nodes[0] > 0)
    {
        stamps.push_back(element(offset, nodes[0], value * s));
        stamps.push_back(element(nodes[0], offset, 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(element(offset, nodes[1], -value * s));
        stamps.push_back(element(nodes[1], offset, -1));
    }
    stamps.push_back(element(offset, offset, -1));
}

void inductor::set_stamp(netlist& nlist)
{
    GiNaC::ex s = get_symbol("s");
    unsigned int offset = nlist.number_of_nodes() + nlist.get_current_impedance();
    nlist.increment_current_impedance();
    if(nodes[0] > 0)
    {
        stamps.push_back(element(offset, nodes[0], 1/(value * s)));
        stamps.push_back(element(nodes[0], offset, 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(element(offset, nodes[1], -1/(value * s)));
        stamps.push_back(element(nodes[1], offset, -1));
    }
    stamps.push_back(element(offset, offset, -1));
}

void voltage_source::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_impedances() + nlist.get_current_voltage_source();
    nlist.increment_current_voltage_source();
    if(nodes[0] > 0)
    {
        stamps.push_back(element(offset, nodes[0], 1));
        stamps.push_back(element(nodes[0], offset, 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(element(offset, nodes[1], -1));
        stamps.push_back(element(nodes[1], offset, -1));
    }
}

void current_source::set_stamp(netlist& nlist)
{
    // a current source has no element (no changes in the network matrix)
    // suppress unused parameter warnings
    (void)nlist;
}

void opamp::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source) + nlist.get_current_opamp();
    if(nodes[0] > 0)
    {
        stamps.push_back(element(offset, nodes[0], 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(element(offset, nodes[1], -1));
    }
    if(nodes[2] > 0)
    {
        stamps.push_back(element(nodes[2], offset, 1));
    }
    nlist.increment_current_opamp();
}

void voltage_controlled_voltage_source::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source | ct_opamp) + nlist.get_current_vcvs();
    if(nodes[0] > 0)
    {
        stamps.push_back(element(offset, nodes[0], 1));
        stamps.push_back(element(nodes[0], offset, 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(element(offset, nodes[1], -1));
        stamps.push_back(element(nodes[1], offset, -1));
    }
    if(nodes[2] > 0)
    {
        stamps.push_back(element(offset, nodes[2], -value));
    }
    if(nodes[3] > 0)
    {
        stamps.push_back(element(offset, nodes[3], value));
    }
    nlist.increment_current_vcvs();
}

void current_controlled_voltage_source::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source | ct_opamp | ct_voltage_controlled_voltage_source) + nlist.get_current_ccvs();
    if(nodes[0] > 0)
    {
        stamps.push_back(element(offset, nodes[0],     1));
        stamps.push_back(element(nodes[0], offset + 1, 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(element(offset, nodes[1],     -1));
        stamps.push_back(element(nodes[1], offset + 1, -1));
    }
    if(nodes[2] > 0)
    {
        stamps.push_back(element(offset + 1, nodes[2], 1));
        stamps.push_back(element(nodes[2], offset,     1));
    }
    if(nodes[3] > 0)
    {
        stamps.push_back(element(offset + 1, nodes[3], -1));
        stamps.push_back(element(nodes[3], offset,     -1));
    }
    stamps.push_back(element(offset, offset, -value));
    nlist.increment_current_ccvs();
}

void voltage_controlled_current_source::set_stamp(netlist& nlist)
{
    // +--+ (regular)
    // +-+- (inverted, what i want!)
    if(nodes[0] > 0)
    {
        if(nodes[2] > 0)
        {
            stamps.push_back(element(nodes[0], nodes[2], -value));
        }
        if(nodes[3] > 0)
        {
            stamps.push_back(element(nodes[0], nodes[3], +value));
        }
    }
    if(nodes[1] > 0)
    {
        if(nodes[2] > 0)
        {
            stamps.push_back(element(nodes[1], nodes[2], +value));
        }
        if(nodes[3] > 0)
        {
            stamps.push_back(element(nodes[1], nodes[3], -value));
        }
    }
    // suppress unused parameter warning
    (void)nlist;
}

void current_controlled_current_source::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source | ct_opamp | ct_voltage_controlled_voltage_source) + 2 * nlist.number_of_devices(ct_current_controlled_voltage_source) + nlist.get_current_cccs();
    if(nodes[0] > 0)
    {
        stamps.push_back(element(nodes[0], offset, value));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(element(nodes[1], offset, -value));
    }
    if(nodes[2] > 0)
    {
        stamps.push_back(element(offset, nodes[2], 1));
        stamps.push_back(element(nodes[2], offset, 1));
    }
    if(nodes[3] > 0)
    {
        stamps.push_back(element(offset, nodes[3], -1));
        stamps.push_back(element(nodes[3], offset, -1));
    }
    nlist.increment_current_ccvs();
}

// vim: nowrap
