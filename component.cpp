#include "component.hpp"
#include "netlist.hpp"

std::unique_ptr<component> create_component(const std::string& line)
{
    char type = line[0];
    switch(type)
    {
        case 'R':
            return std::make_unique<resistor>(line);
        case 'C':
            return std::make_unique<capacitor>(line);
        case 'L':
            return std::make_unique<inductor>(line);
        case 'V':
            return std::make_unique<voltage_source>(line);
        case 'I':
            return std::make_unique<current_source>(line);
        case 'E':
            return std::make_unique<voltage_controlled_voltage_source>(line);
        case 'O':
            return std::make_unique<opamp>(line);
        case 'F':
            return std::make_unique<current_controlled_voltage_source>(line);
        case 'G':
            return std::make_unique<voltage_controlled_current_source>(line);
        case 'H':
            return std::make_unique<current_controlled_current_source>(line);
        default:
            std::cerr << "Unknown component: '" << type << "'\n";
            return NULL;
    }
}

void impedance::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.get_current_impedance();
    nlist.increment_current_impedance();
    if(nodes[0] > 0)
    {
        stamps.push_back(stamp(offset, nodes[0], value));
        stamps.push_back(stamp(nodes[0], offset, 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(stamp(offset, nodes[1], -value));
        stamps.push_back(stamp(nodes[1], offset, -1));
    }
    stamps.push_back(stamp(offset, offset, -1));
}

void voltage_source::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_impedances() + nlist.get_current_voltage_source();
    nlist.increment_current_voltage_source();
    if(nodes[0] > 0)
    {
        stamps.push_back(stamp(offset, nodes[0], 1));
        stamps.push_back(stamp(nodes[0], offset, 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(stamp(offset, nodes[1], -1));
        stamps.push_back(stamp(nodes[1], offset, -1));
    }
}

void current_source::set_stamp(netlist& nlist)
{
    // a current source has no stamp (no changes in the network matrix)
    // suppress unused parameter warnings
    (void)nlist;
}

void opamp::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source) + nlist.get_current_opamp();
    if(nodes[0] > 0)
    {
        stamps.push_back(stamp(offset, nodes[0], 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(stamp(offset, nodes[1], -1));
    }
    if(nodes[2] > 0)
    {
        stamps.push_back(stamp(nodes[2], offset, 1));
    }
    nlist.increment_current_opamp();
}

void voltage_controlled_voltage_source::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source | ct_opamp) + nlist.get_current_vcvs();
    if(nodes[0] > 0)
    {
        stamps.push_back(stamp(offset, nodes[0], 1));
        stamps.push_back(stamp(nodes[0], offset, 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(stamp(offset, nodes[1], -1));
        stamps.push_back(stamp(nodes[1], offset, -1));
    }
    if(nodes[2] > 0)
    {
        stamps.push_back(stamp(offset, nodes[2], -value));
    }
    if(nodes[3] > 0)
    {
        stamps.push_back(stamp(offset, nodes[3], value));
    }
    nlist.increment_current_vcvs();
}

void current_controlled_voltage_source::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source | ct_opamp | ct_voltage_controlled_voltage_source) + nlist.get_current_ccvs();
    if(nodes[0] > 0)
    {
        stamps.push_back(stamp(offset, nodes[0],     1));
        stamps.push_back(stamp(nodes[0], offset + 1, 1));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(stamp(offset, nodes[1],     -1));
        stamps.push_back(stamp(nodes[1], offset + 1, -1));
    }
    if(nodes[2] > 0)
    {
        stamps.push_back(stamp(offset + 1, nodes[2], 1));
        stamps.push_back(stamp(nodes[2], offset,     1));
    }
    if(nodes[3] > 0)
    {
        stamps.push_back(stamp(offset + 1, nodes[3], -1));
        stamps.push_back(stamp(nodes[3], offset,     -1));
    }
    stamps.push_back(stamp(offset, offset, -value));
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
            stamps.push_back(stamp(nodes[0], nodes[2], -value));
        }
        if(nodes[3] > 0)
        {
            stamps.push_back(stamp(nodes[0], nodes[3], +value));
        }
    }
    if(nodes[1] > 0)
    {
        if(nodes[2] > 0)
        {
            stamps.push_back(stamp(nodes[1], nodes[2], +value));
        }
        if(nodes[3] > 0)
        {
            stamps.push_back(stamp(nodes[1], nodes[3], -value));
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
        stamps.push_back(stamp(nodes[0], offset, value));
    }
    if(nodes[1] > 0)
    {
        stamps.push_back(stamp(nodes[1], offset, -value));
    }
    if(nodes[2] > 0)
    {
        stamps.push_back(stamp(offset, nodes[2], 1));
        stamps.push_back(stamp(nodes[2], offset, 1));
    }
    if(nodes[3] > 0)
    {
        stamps.push_back(stamp(offset, nodes[3], -1));
        stamps.push_back(stamp(nodes[3], offset, -1));
    }
    nlist.increment_current_ccvs();
}

// vim: nowrap
