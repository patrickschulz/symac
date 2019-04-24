#include "mna.hpp"

#include "stamp.hpp"
#include "device_stamps.hpp"
#include "../component.hpp"
#include "../nodemap.hpp"
#include "../componentlist.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <boost/format.hpp>

unsigned int get_element_size(const component& c)
{
    stamp stmp = get_stamp(c);
    return stmp.size() - c.get_nodes().size();
}

static unsigned int get_networksize(const componentlist& components)
{
    unsigned int networksize = 0;
    // find network size
    for(const auto& c : components)
    {
        networksize += get_element_size(c);
    }
    networksize += components.number_of_nodes();
    return networksize;
}

namespace mna
{
    GiNaC::matrix create_A_matrix(nodemap& nmap, const componentlist& components)
    {
        unsigned int networksize = get_networksize(components);

        GiNaC::matrix A(networksize, networksize);

        unsigned int offset = components.number_of_nodes();
        for(const auto& c : components)
        {
            auto nodes = nmap[c.get_nodes()];
            stamp stmp = get_stamp(c);
            for(const element& elem : stmp)
            {
                unsigned int row = 0;
                unsigned int col = 0;
                if(elem.get_row() > nodes.size())
                {
                    row = offset + elem.get_row() - nodes.size();
                }
                else
                {
                    row = nodes[elem.get_row() - 1];
                }

                if(elem.get_column() > nodes.size())
                {
                    col = offset + elem.get_column() - nodes.size();
                }
                else
                {
                    col = nodes[elem.get_column() - 1];
                }

                if(row > 0 && col > 0)
                {
                    // GiNaC matrices are 0-indexed, so we need to decrement the indices by 1
                    A(row - 1, col - 1) += elem.get_value();
                }
            }
            offset += get_element_size(c);
        }

        return A;
    }

    GiNaC::matrix create_x_vector(nodemap& nmap, const componentlist& components)
    {
        unsigned int networksize = get_networksize(components);
        GiNaC::matrix x(networksize, 1);

        unsigned int row = 0;
        for(; row < components.number_of_nodes(); ++row)
        {
            std::string node = nmap[row + 1];
            x(row, 0) = get_symbol(node);
        }        

        for(const auto& c : components)
        {
            for(unsigned int j = 0; j < get_element_size(c); ++j)
            {
                boost::format fmter = boost::format("I%s") % (c.get_name());
                std::string str = fmter.str();
                if(get_element_size(c) > 1)
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
        unsigned int networksize = get_networksize(components);
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
            offset += get_element_size(c);
        }

        return z;
    }
}
