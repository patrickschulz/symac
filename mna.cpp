#include "netlist.hpp"
#include "mna.hpp"
#include "stamp.hpp"

#include <string>
#include <boost/format.hpp>

namespace mna {

    GiNaC::matrix create_A_matrix(const netlist& nlist)
    {
        unsigned int networksize = nlist.full_network_size();
        GiNaC::matrix A(networksize, networksize);
        for(const auto& c : nlist.get_components())
        {
            for(const element& elem : c->get_stamp())
            {
                // GiNaC matrices are 0-indexed, so we need to decrement the indices by 1
                A(elem.get_row() - 1, elem.get_column() - 1) += elem.get_value();
            }
        }
        return A;
    }

    GiNaC::matrix create_x_vector(const netlist& nlist)
    {
        unsigned int networksize = nlist.full_network_size();
        GiNaC::matrix x(networksize, 1);

        unsigned int row = 0;
        for(; row < nlist.number_of_nodes(); ++row)
        {
            boost::format fmter = boost::format("v%d") % (row + 1);
            std::string str = fmter.str();
            x(row, 0) = get_symbol(str);
        }        

        const auto& components = nlist.get_components();
        unsigned int offset = nlist.number_of_nodes();
        for(unsigned int i = 0; i < components.size(); ++i)
        {
            boost::format fmter = boost::format("I%d") % (i + 1);
            std::string str = fmter.str();
            x(offset, 0) = get_symbol(str);
            offset += components[i]->element_size();
        }

        return x;
    }

    GiNaC::matrix create_z_vector(const netlist& nlist)
    {
        unsigned int networksize = nlist.full_network_size();
        GiNaC::matrix z(networksize, 1);

        const auto& components = nlist.get_components();
        unsigned int offset = nlist.number_of_nodes();
        for(unsigned int i = 0; i < components.size(); ++i)
        {
            const auto& c = components[i];
            if(c->type() == ct_voltage_source)
            {
                z(offset + i, 0) += c->get_value();
            }
            if(c->type() == ct_current_source)
            {
                std::vector<unsigned int> nodes = c->get_nodes();
                if(nodes[0] > 0)
                {
                    z(nodes[0] - 1, 0) += c->get_value();
                }
                if(nodes[1] > 0)
                {
                    z(nodes[1] - 1, 0) -= c->get_value();
                }
            }
        }

        return z;
    }
}
