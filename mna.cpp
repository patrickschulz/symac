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
        for(auto c : nlist.get_devices(ct_resistor | ct_capacitor | ct_inductor))
        {
            char shorttype = c->short_type();
            boost::format fmter = boost::format("I%c%d") % shorttype % (row + 1 - nlist.number_of_nodes());
            std::string str = fmter.str();
            x(row, 0) = get_symbol(str);
            ++row;
        }
        for(unsigned int i = 0; i < nlist.number_of_devices(ct_voltage_source); ++i)
        {
            boost::format fmter = boost::format("Iv%d") % (i + 1);
            std::string str = fmter.str();
            x(row, 0) = get_symbol(str);
            ++row;
        }
        for(unsigned int i = 0; i < nlist.number_of_devices(ct_opamp); ++i)
        {
            boost::format fmter = boost::format("Iop%d") % (i + 1);
            std::string str = fmter.str();
            x(row, 0) = get_symbol(str);
            ++row;
        }
        for(unsigned int i = 0; i < nlist.number_of_devices(ct_voltage_controlled_voltage_source); ++i)
        {
            boost::format fmter = boost::format("Ie%d") % (i + 1);
            std::string str = fmter.str();
            x(row, 0) = get_symbol(str);
            ++row;
        }
        for(unsigned int i = 0; i < nlist.number_of_devices(ct_current_controlled_voltage_source); i += 2)
        {
            boost::format fmter1 = boost::format("Ifin%d") % (i + 1);
            std::string str1 = fmter1.str();
            boost::format fmter2 = boost::format("Ifout%d") % (i + 1);
            std::string str2 = fmter2.str();
            x(row, 0) = get_symbol(str1);
            x(row + 1, 0) = get_symbol(str2);
            row += 2;
        }
        for(unsigned int i = 0; i < nlist.number_of_devices(ct_current_controlled_current_source); ++i)
        {
            boost::format fmter = boost::format("Ihin%d") % (i + 1);
            std::string str = fmter.str();
            x(row, 0) = get_symbol(str);
            ++row;
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
