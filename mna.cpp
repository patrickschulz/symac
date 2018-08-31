#include "component.hpp"
#include "mna.hpp"
#include "stamp.hpp"

#include <string>
#include <boost/format.hpp>

namespace mna {

    GiNaC::matrix create_A_matrix(unsigned int networksize, const std::vector<std::unique_ptr<component>>& components)
    {
        GiNaC::matrix A(networksize, networksize);
        for(const auto& c : components)
        {
            for(const element& elem : c->get_stamp())
            {
                // GiNaC matrices are 0-indexed, so we need to decrement the indices by 1
                A(elem.get_row() - 1, elem.get_column() - 1) += elem.get_value();
            }
        }
        return A;
    }

    GiNaC::matrix create_x_vector(unsigned int networksize, unsigned int number_of_nodes, const std::vector<std::unique_ptr<component>>& components)
    {
        GiNaC::matrix x(networksize, 1);

        unsigned int row = 0;
        for(; row < number_of_nodes; ++row)
        {
            boost::format fmter = boost::format("v%d") % (row + 1);
            std::string str = fmter.str();
            x(row, 0) = get_symbol(str);
        }        

        unsigned int offset = number_of_nodes;
        for(unsigned int i = 0; i < components.size(); ++i)
        {
            boost::format fmter = boost::format("I%d") % (i + 1);
            std::string str = fmter.str();
            x(offset, 0) = get_symbol(str);
            offset += components[i]->element_size();
        }

        return x;
    }

    GiNaC::matrix create_z_vector(unsigned int networksize, unsigned int number_of_nodes, const std::vector<std::unique_ptr<component>>& components)
    {
        GiNaC::matrix z(networksize, 1);

        unsigned int offset = number_of_nodes;
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
