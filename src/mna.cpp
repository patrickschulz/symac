#include "component.hpp"
#include "mna.hpp"
#include "stamp.hpp"
#include "nodemap.hpp"
#include "componentlist.hpp"

#include <string>
#include <boost/format.hpp>

static stamp get_stamp(const component& c, unsigned int offset, nodemap& nmap)
{
    std::vector<unsigned int> nodes = nmap[c.get_nodes()];
    GiNaC::ex value = c.get_value();
    stamp stmp;
    GiNaC::ex s = get_symbol("s");
    switch(c.get_type())
    {
        case ct_resistor:
        case ct_capacitor:
        case ct_inductor:
            if(c.get_type() == ct_capacitor)
            {
                value = 1 / (s * value);
            }
            if(c.get_type() == ct_inductor)
            {
                value = s * value;
            }
            // impedance-based
            stmp.write(offset, nodes[0], 1);
            stmp.write(nodes[0], offset, 1);
            stmp.write(offset, nodes[1], -1);
            stmp.write(nodes[1], offset, -1);
            stmp.write(offset, offset, -value);

            // admittance-based
            /*
            value = 1 / value;
            stmp.write(offset, nodes[0], value);
            stmp.write(nodes[0], offset, 1);
            stmp.write(offset, nodes[1], -value);
            stmp.write(nodes[1], offset, -1);
            stmp.write(offset, offset, -1);
            */
            break;
        case ct_voltage_source:
            stmp.write(offset, nodes[0], 1);
            stmp.write(nodes[0], offset, 1);
            stmp.write(offset, nodes[1], -1);
            stmp.write(nodes[1], offset, -1);
            break;
        case ct_opamp:
            stmp.write(offset, nodes[0], 1);
            stmp.write(offset, nodes[1], -1);
            stmp.write(nodes[2], offset, 1);
            //stmp.write(nodes[3], offset, -1); // TODO: use this line for four-terminal (differential) opamps?
            break;
        case ct_voltage_controlled_voltage_source:
            stmp.write(offset, nodes[0], 1);
            stmp.write(nodes[0], offset, 1);
            stmp.write(offset, nodes[1], -1);
            stmp.write(nodes[1], offset, -1);
            stmp.write(offset, nodes[2], -value);
            stmp.write(offset, nodes[3], value);
            break;
        case ct_current_controlled_voltage_source:
            stmp.write(offset, nodes[0],     1);
            stmp.write(nodes[0], offset + 1, 1);
            stmp.write(offset, nodes[1],     -1);
            stmp.write(nodes[1], offset + 1, -1);
            stmp.write(offset + 1, nodes[2], 1);
            stmp.write(nodes[2], offset,     1);
            stmp.write(offset + 1, nodes[3], -1);
            stmp.write(nodes[3], offset,     -1);
            stmp.write(offset, offset, -value);
            break;
        case ct_voltage_controlled_current_source:
            stmp.write(nodes[0], nodes[2], -value);
            stmp.write(nodes[0], nodes[3], +value);
            stmp.write(nodes[1], nodes[2], +value);
            stmp.write(nodes[1], nodes[3], -value);
            break;
        case ct_current_controlled_current_source:
            stmp.write(nodes[0], offset, -value);
            stmp.write(nodes[1], offset,  value);
            stmp.write(offset, nodes[2], 1);
            stmp.write(nodes[2], offset, 1);
            stmp.write(offset, nodes[3], -1);
            stmp.write(nodes[3], offset, -1);
            break;
        case ct_none:
        case ct_current_source:
            // suppress warning
            break;
    }
    return stmp;
}

namespace mna {

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

        unsigned int i = 0;
        for(const auto& c : components)
        {
            for(unsigned int j = 0; j < c.element_size(); ++j)
            {
                boost::format fmter = boost::format("I%d") % (i + j + 1);
                ++i;
                std::string str = fmter.str();
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
            if(c.get_type() == ct_voltage_source)
            {
                z(offset, 0) += c.get_value();
            }
            if(c.get_type() == ct_current_source)
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
