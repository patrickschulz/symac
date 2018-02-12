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

void component::reset_stamp()
{
    stamps.clear();
}

void resistor::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.get_current_impedance();
    nlist.increment_current_impedance();
    if(node1 > 0)
    {
        stamps.push_back(stamp(offset, node1, 1/value));
        stamps.push_back(stamp(node1, offset, 1));
    }
    if(node2 > 0)
    {
        stamps.push_back(stamp(offset, node2, -1/value));
        stamps.push_back(stamp(node2, offset, -1));
    }
    stamps.push_back(stamp(offset, offset, -1));
}

void capacitor::set_stamp(netlist& nlist)
{
    GiNaC::ex s = get_symbol("s");
    unsigned int offset = nlist.number_of_nodes() + nlist.get_current_impedance();
    nlist.increment_current_impedance();
    if(node1 > 0)
    {
        stamps.push_back(stamp(offset, node1, value * s));
        stamps.push_back(stamp(node1, offset, 1));
    }
    if(node2 > 0)
    {
        stamps.push_back(stamp(offset, node2, -value * s));
        stamps.push_back(stamp(node2, offset, -1));
    }
    stamps.push_back(stamp(offset, offset, -1));
}

void inductor::set_stamp(netlist& nlist)
{
    GiNaC::ex s = get_symbol("s");
    unsigned int offset = nlist.number_of_nodes() + nlist.get_current_impedance();
    nlist.increment_current_impedance();
    if(node1 > 0)
    {
        stamps.push_back(stamp(offset, node1, 1/(value * s)));
        stamps.push_back(stamp(node1, offset, 1));
    }
    if(node2 > 0)
    {
        stamps.push_back(stamp(offset, node2, -1/(value * s)));
        stamps.push_back(stamp(node2, offset, -1));
    }
    stamps.push_back(stamp(offset, offset, -1));
}

void voltage_source::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_impedances() + nlist.get_current_voltage_source();
    nlist.increment_current_voltage_source();
    if(node1 > 0)
    {
        stamps.push_back(stamp(offset, node1, 1));
        stamps.push_back(stamp(node1, offset, 1));
    }
    if(node2 > 0)
    {
        stamps.push_back(stamp(offset, node2, -1));
        stamps.push_back(stamp(node2, offset, -1));
    }
}

void current_source::set_stamp(netlist& nlist)
{
    // a current source has no stamp (no changes in the network matrix)
}

void opamp::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source) + nlist.get_current_opamp();
    if(node1 > 0)
    {
        stamps.push_back(stamp(offset, node1, 1));
    }
    if(node2 > 0)
    {
        stamps.push_back(stamp(offset, node2, -1));
    }
    if(node3 > 0)
    {
        stamps.push_back(stamp(node3, offset, 1));
    }
    nlist.increment_current_opamp();
}

void voltage_controlled_voltage_source::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source | ct_opamp) + nlist.get_current_vcvs();
    if(node1 > 0)
    {
        stamps.push_back(stamp(offset, node1, 1));
        stamps.push_back(stamp(node1, offset, 1));
    }
    if(node2 > 0)
    {
        stamps.push_back(stamp(offset, node2, -1));
        stamps.push_back(stamp(node2, offset, -1));
    }
    if(node3 > 0)
    {
        stamps.push_back(stamp(offset, node3, -value));
    }
    if(node4 > 0)
    {
        stamps.push_back(stamp(offset, node4, value));
    }
    nlist.increment_current_vcvs();
}

void current_controlled_voltage_source::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source | ct_opamp | ct_voltage_controlled_voltage_source) + nlist.get_current_ccvs();
    if(node1 > 0)
    {
        stamps.push_back(stamp(offset, node1,     1));
        stamps.push_back(stamp(node1, offset + 1, 1));
    }
    if(node2 > 0)
    {
        stamps.push_back(stamp(offset, node2,     -1));
        stamps.push_back(stamp(node2, offset + 1, -1));
    }
    if(node3 > 0)
    {
        stamps.push_back(stamp(offset + 1, node3, 1));
        stamps.push_back(stamp(node3, offset,     1));
    }
    if(node4 > 0)
    {
        stamps.push_back(stamp(offset + 1, node4, -1));
        stamps.push_back(stamp(node4, offset,     -1));
    }
    stamps.push_back(stamp(offset, offset, -value));
    nlist.increment_current_ccvs();
}

void voltage_controlled_current_source::set_stamp(netlist& nlist)
{
    // +--+ (regular)
    // +-+- (inverted, what i want!)
    if(node1 > 0)
    {
        if(node3 > 0)
        {
            stamps.push_back(stamp(node1, node3, -value));
        }
        if(node4 > 0)
        {
            stamps.push_back(stamp(node1, node4, +value));
        }
    }
    if(node2 > 0)
    {
        if(node3 > 0)
        {
            stamps.push_back(stamp(node2, node3, +value));
        }
        if(node4 > 0)
        {
            stamps.push_back(stamp(node2, node4, -value));
        }
    }
}

void current_controlled_current_source::set_stamp(netlist& nlist)
{
    unsigned int offset = nlist.number_of_nodes() + nlist.number_of_devices(ct_resistor | ct_capacitor | ct_inductor | ct_voltage_source | ct_opamp | ct_voltage_controlled_voltage_source) + 2 * nlist.number_of_devices(ct_current_controlled_voltage_source) + nlist.get_current_cccs();
    if(node1 > 0)
    {
        stamps.push_back(stamp(node1, offset, value));
    }
    if(node2 > 0)
    {
        stamps.push_back(stamp(node2, offset, -value));
    }
    if(node3 > 0)
    {
        stamps.push_back(stamp(offset, node3, 1));
        stamps.push_back(stamp(node3, offset, 1));
    }
    if(node4 > 0)
    {
        stamps.push_back(stamp(offset, node4, -1));
        stamps.push_back(stamp(node4, offset, -1));
    }
    nlist.increment_current_ccvs();
}

// vim: nowrap
