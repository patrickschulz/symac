#include "mna.hpp"

#include "../component.hpp"
#include "../config.hpp"
#include "../stamp.hpp"
#include "../nodemap.hpp"
#include "../componentlist.hpp"

#include <string>
#include <vector>
#include <boost/format.hpp>

static stamp get_impedance_stamp(const GiNaC::ex& value, unsigned int offset, const std::vector<unsigned int>& nodes)
{
    // suppress warning
    (void) offset;

    stamp stmp;
    // G-Matrix
    stmp.write(nodes[0], nodes[0],  1 / value);
    stmp.write(nodes[1], nodes[1],  1 / value);
    stmp.write(nodes[0], nodes[1], -1 / value);
    stmp.write(nodes[1], nodes[0], -1 / value);
    /* Impedance approach
    stmp.write(offset, nodes[0], 1);
    stmp.write(nodes[0], offset, 1);
    stmp.write(offset, nodes[1], -1);
    stmp.write(nodes[1], offset, -1);
    stmp.write(offset, offset, -value);
    */
    return stmp;
}

static stamp get_conductance_stamp(const GiNaC::ex& value, unsigned int offset, const std::vector<unsigned int>& nodes)
{
    // suppress warning
    (void) offset;

    stamp stmp;
    // G-Matrix
    stmp.write(nodes[0], nodes[0],  value);
    stmp.write(nodes[1], nodes[1],  value);
    stmp.write(nodes[0], nodes[1], -value);
    stmp.write(nodes[1], nodes[0], -value);
    /* Admittance-aproach
    stmp.write(offset, nodes[0], value);
    stmp.write(nodes[0], offset, 1);
    stmp.write(offset, nodes[1], -value);
    stmp.write(nodes[1], offset, -1);
    stmp.write(offset, offset, -1);
    */
    return stmp;
}

static stamp get_voltage_source_stamp(unsigned int offset, const std::vector<unsigned int>& nodes)
{
    stamp stmp;
    stmp.write(offset, nodes[0], 1);
    stmp.write(nodes[0], offset, 1);
    stmp.write(offset, nodes[1], -1);
    stmp.write(nodes[1], offset, -1);
    return stmp;
}

static stamp get_vcvs_stamp(const GiNaC::ex& value, unsigned int offset, const std::vector<unsigned int>& nodes)
{
    stamp stmp;
    stmp.write(offset, nodes[0], 1);
    stmp.write(nodes[0], offset, 1);
    stmp.write(offset, nodes[1], -1);
    stmp.write(nodes[1], offset, -1);
    stmp.write(offset, nodes[2], -value);
    stmp.write(offset, nodes[3], value);
    return stmp;
}

static stamp get_ccvs_stamp(const GiNaC::ex& value, unsigned int offset, const std::vector<unsigned int>& nodes)
{
    stamp stmp;
    stmp.write(offset, nodes[0],     1);
    stmp.write(nodes[0], offset + 1, 1);
    stmp.write(offset, nodes[1],     -1);
    stmp.write(nodes[1], offset + 1, -1);
    stmp.write(offset + 1, nodes[2], 1);
    stmp.write(nodes[2], offset,     1);
    stmp.write(offset + 1, nodes[3], -1);
    stmp.write(nodes[3], offset,     -1);
    stmp.write(offset, offset, -value);
    return stmp;
}

static stamp get_vccs_stamp(const GiNaC::ex& value, unsigned int offset, const std::vector<unsigned int>& nodes)
{
    // suppress warning
    (void) offset;

    stamp stmp;
    stmp.write(nodes[0], nodes[2], -value);
    stmp.write(nodes[0], nodes[3], +value);
    stmp.write(nodes[1], nodes[2], +value);
    stmp.write(nodes[1], nodes[3], -value);
    return stmp;
}

static stamp get_cccs_stamp(const GiNaC::ex& value, unsigned int offset, const std::vector<unsigned int>& nodes)
{
    stamp stmp;
    stmp.write(nodes[0], offset, -value);
    stmp.write(nodes[1], offset,  value);
    stmp.write(offset, nodes[2], 1);
    stmp.write(nodes[2], offset, 1);
    stmp.write(offset, nodes[3], -1);
    stmp.write(nodes[3], offset, -1);
    return stmp;
}

static stamp get_opamp_stamp(unsigned int offset, const std::vector<unsigned int>& nodes)
{

    stamp stmp;
    stmp.write(offset, nodes[0], 1);
    stmp.write(offset, nodes[1], -1);
    stmp.write(nodes[2], offset, 1);
    //stmp.write(nodes[3], offset, -1); // TODO: use this line for four-terminal (differential) opamps?
    return stmp;
}

static stamp get_stamp(const component& c, unsigned int offset, nodemap& nmap)
{
    std::vector<unsigned int> nodes = nmap[c.get_nodes()];
    GiNaC::ex value = c.get_value();
    switch(c.get_type())
    {
        case ct_resistor:
            return get_impedance_stamp(value, offset, nodes);
        case ct_inductor:
            return get_impedance_stamp(value * get_complex_symbol("s"), offset, nodes);
        case ct_conductor:
            return get_conductance_stamp(value, offset, nodes);
        case ct_capacitor:
            return get_conductance_stamp(value * get_complex_symbol("s"), offset, nodes);
        case ct_voltage_source:
            return get_voltage_source_stamp(offset, nodes);
        case ct_opamp:
            return get_opamp_stamp(offset, nodes);
        case ct_voltage_controlled_voltage_source:
            return get_vcvs_stamp(value, offset, nodes);
        case ct_current_controlled_voltage_source:
            return get_ccvs_stamp(value, offset, nodes);
        case ct_voltage_controlled_current_source:
            return get_vccs_stamp(value, offset, nodes);
        case ct_current_controlled_current_source:
            return get_cccs_stamp(value, offset, nodes);
        default: // suppress warning for sources that don't need any stamp (ct_port and ct_current_source)
            return stamp();
    }
}

namespace mna
{
    GiNaC::matrix create_A_matrix(nodemap& nmap, const componentlist& components)
    {
        unsigned int networksize = components.network_size();
        GiNaC::matrix A(networksize, networksize);
        unsigned int offset = components.number_of_nodes() + 1;
        for(const auto& c : components)
        {
            stamp stmp = get_stamp(c, offset, nmap);
            offset += c.element_size();
            for(const element& elem : stmp)
            {
                // GiNaC matrices are 0-indexed, so we need to decrement the indices by 1
                A(elem.get_row() - 1, elem.get_column() - 1) += elem.get_value();
            }
        }
        return A;
    }

    GiNaC::matrix create_x_vector(const componentlist& components)
    {
        unsigned int networksize = components.network_size();
        GiNaC::matrix x(networksize, 1);

        unsigned int row = 0;
        for(; row < components.number_of_nodes(); ++row)
        {
            boost::format fmter = boost::format("v%d") % (row + 1);
            std::string str = fmter.str();
            x(row, 0) = get_symbol(str);
        }        

        for(const auto& c : components)
        {
            for(unsigned int j = 0; j < c.element_size(); ++j)
            {
                boost::format fmter = boost::format("I%s") % (c.get_name());
                std::string str = fmter.str();
                if(c.element_size() > 1)
                {
                    boost::format apxfmt = boost::format("%d") % (j);
                    std::string appendix = apxfmt.str();
                }
                x(row, 0) = get_symbol(str);
                ++row;
            }
        }

        return x;
    }

    GiNaC::matrix create_z_vector(nodemap& nmap, const componentlist& components)
    {
        unsigned int networksize = components.network_size();
        GiNaC::matrix z(networksize, 1);

        unsigned int offset = components.number_of_nodes();
        for(const auto& c : components)
        {
            if(c == ct_voltage_source)
            {
                z(offset, 0) += c.get_value();
            }
            if(c == ct_current_source)
            {
                std::vector<unsigned int> nodes = nmap[c.get_nodes()];
                if(nodes[0] > 0)
                {
                    z(nodes[0] - 1, 0) += c.get_value();
                }
                if(nodes[1] > 0)
                {
                    z(nodes[1] - 1, 0) -= c.get_value();
                }
            }
            offset += c.element_size();
        }

        return z;
    }
}
