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
            for(const stamp& st : c->get_stamps())
            {
                // GiNaC matrices are 0-indexed, so we need to decrement the indices by 1
                A(st.get_row() - 1, st.get_column() - 1) += st.get_value();
            }
        }
        return A;
    }

    GiNaC::matrix create_x_vector(const netlist& nlist)
    {
        /*
           The x vector contains the unknown quantities in the network, which are to be calculated. From top to bottom:
           - node voltages
           - currents through passives (resistors, capacitors and inductors)
           - current through voltage sources
           - output currents of opamps
           - current through voltage controlled voltage sources
           - input current of current controlled voltage sources
           - output current of current controlled voltage sources
           - input current of current controlled current sources
           */
        unsigned int networksize = nlist.full_network_size();
        GiNaC::matrix x(networksize, 1);

        unsigned int row = 0;
        for(; row < nlist.number_of_nodes(); ++row)
        {
            boost::format fmter = boost::format("v%d") % (row + 1);
            std::string str = fmter.str();
            x(row, 0) = get_symbol(str);
        }

        std::vector<std::tuple<component_types, std::vector<std::string>>> dev_formats = {
            std::make_tuple<component_types, std::vector<std::string>>(ct_resistor | ct_capacitor | ct_inductor, {"Iz"}),
            std::make_tuple<component_types, std::vector<std::string>>(ct_voltage_source, {"Iv"}),
            std::make_tuple<component_types, std::vector<std::string>>(ct_opamp, {"Iop"}),
            std::make_tuple<component_types, std::vector<std::string>>(ct_voltage_controlled_voltage_source, {"Ie"}),
            std::make_tuple<component_types, std::vector<std::string>>(ct_current_controlled_current_source, {"Ihin"}),
            std::make_tuple<component_types, std::vector<std::string>>(ct_current_controlled_voltage_source, {"Ifin", "Ifout"})
        };

        component_types prev_components = ct_none;
        for(const auto& dev : dev_formats)
        {
            component_types ct;
            std::vector<std::string> currents;
            std::tie(ct, currents) = dev;

            for(unsigned int i = 0; i < nlist.number_of_devices(ct); ++i)
            {
                //for(const auto& current : currents)
                for(unsigned int cur = 0; cur < currents.size(); ++cur)
                {
                    std::string current = currents[cur];
                    unsigned int num = nlist.number_of_nodes() + nlist.number_of_devices(prev_components);
                    boost::format fmter = boost::format("%s%d") % current % (row + 1 - num);
                    std::string str = fmter.str();
                    x(row + cur, 0) = get_symbol(str);
                }
                ++row;
            }
            prev_components = prev_components | ct;
        }

        return x;
    }

    GiNaC::matrix create_z_vector(const netlist& nlist)
    {
        unsigned int networksize = nlist.full_network_size();
        GiNaC::matrix z(networksize, 1);

        // current sources
        for(auto c : nlist.get_devices(ct_current_source))
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
        // voltage sources
        unsigned int offset = nlist.number_of_nodes() + nlist.number_of_impedances();
        unsigned int row = 0;
        for(auto c : nlist.get_devices(ct_voltage_source))
        {
            z(row + offset, 0) += c->get_value();
            ++row;
        }

        return z;
    }
}
