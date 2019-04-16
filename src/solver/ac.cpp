#include "ac.hpp"

#include "../symbol.hpp"

#include <boost/format.hpp>

static GiNaC::ex get_value_passive_device(const component& c)
{
    GiNaC::ex value = c.get_value();
    GiNaC::ex s = get_complex_symbol("s");
    switch(c.get_type())
    {
        case ct_resistor:
            return 1 / value;
        case ct_conductor:
            return value;
        case ct_capacitor:
            return s * value;
        case ct_inductor:
            return 1 / s / value;
        default: // never reached
            assert(0);
            return 0;
    }
}

void solve_ac(const componentlist& components, nodemap& nmap, result& results, bool linearize, bool print)
{
    GiNaC::matrix res = solve_network(components, nmap, linearize, print);

    // voltages
    for(unsigned int row = 0; row < components.number_of_nodes(); ++row)
    {
        std::string usernode = nmap[row + 1];
        results.add("V", usernode, res(row, 0));
    }

    // currents
    // currents must be calculated in two ways:
    //  - derived currents, by using the node voltages and the device values (e.g. resistors)
    //  - simulated currents for voltage sources, op amps etc.
    for(const component& c : components)
    {
        switch(c.get_type())
        {
            case ct_resistor:
            case ct_conductor:
            case ct_inductor:
            case ct_capacitor:
            {
                GiNaC::ex value = get_value_passive_device(c);
                std::vector<std::string> nodes = c.get_nodes();
                GiNaC::ex voltage = results.get("V", nodes[0]) - results.get("V", nodes[1]);
                boost::format fmter = boost::format("%s.%s");
                results.add("I", str(fmter % c.get_name() % "p"),  voltage * value);
                results.add("I", str(fmter % c.get_name() % "n"), -voltage * value);
                break;
            }
            case ct_voltage_controlled_current_source:
            {
                GiNaC::ex value = c.get_value();
                std::vector<std::string> nodes = c.get_nodes();
                GiNaC::ex voltage = results.get("V", nodes[2]) - results.get("V", nodes[3]);
                boost::format fmter = boost::format("%s.%s");
                results.add("I", str(fmter % c.get_name() % "p"),  voltage * value);
                results.add("I", str(fmter % c.get_name() % "n"), -voltage * value);
                break;
            }
            case ct_current_source:
            {
                boost::format fmter = boost::format("%s.%s");
                results.add("I", str(fmter % c.get_name() % "p"),  c.get_value());
                results.add("I", str(fmter % c.get_name() % "n"), -c.get_value());
                break;
            }
            case ct_voltage_source:
            case ct_opamp:
            case ct_voltage_controlled_voltage_source:
            case ct_current_controlled_current_source:
            {
                unsigned int offset = components.number_of_nodes() + components.component_index(c);
                boost::format fmter = boost::format("%s.%s");
                results.add("I", str(fmter % c.get_name() % "p"),  res(offset, 0));
                results.add("I", str(fmter % c.get_name() % "n"), -res(offset, 0));
                break;
            }
            case ct_current_controlled_voltage_source:
            {
                unsigned int offset = components.number_of_nodes() + components.component_index(c);
                boost::format fmter = boost::format("%s.%s");
                results.add("I", str(fmter % c.get_name() % "cp"),  res(offset, 0));
                results.add("I", str(fmter % c.get_name() % "cn"), -res(offset, 0));
                results.add("I", str(fmter % c.get_name() % "p"),  res(offset + 1, 0));
                results.add("I", str(fmter % c.get_name() % "n"), -res(offset + 1, 0));
                break;
            }
            case ct_port:
            case ct_mosfet:
                // auxiliary devices which have no currents
                break;
        }
    }
}
