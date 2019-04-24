#include "nport.hpp"

#include "common.hpp"
#include "../symbol.hpp"

#include <string>
#include <boost/format.hpp>

const std::string portdummynode("DUMMYPORTNODE");

GiNaC::ex get_port_voltage(const component& pp, const GiNaC::matrix& res, nodemap& nmap)
{
    unsigned int node1 = nmap[pp.get_nodes()[0]];
    unsigned int node2 = nmap[pp.get_nodes()[1]];
    GiNaC::ex value = 0;
    if(node1 != 0)
    {
        value = res(node1 - 1, 0);
    }
    if(node2 != 0)
    {
        value = value - res(node2 - 1, 0);
    }
    return value;
}

GiNaC::ex get_nport_numerator(port_mode mode, unsigned int row, unsigned int col, const componentlist& components, const GiNaC::matrix& res, nodemap& nmap)
{
    std::vector<component> ports = components.get_components_by_type(ct_port);
    switch(mode)
    {
        case zport:
            if(col == row)
            {
                return ports[col].get_value();
            }
            else
            {
                return get_port_voltage(ports[row], res, nmap);
            }
        case yport:
            if(col == row)
            {
                return ports[col].get_value();
            }
            else
            {
                return -res(res.rows() - 1, 0);
            }
        case sport:
        {
            component p = ports[row];
            unsigned int inodep = nmap[p.get_nodes()[0]];

            GiNaC::ex vj, ij;
            GiNaC::possymbol Z0 = get_symbol("Z0");

            if(col == row)
            {
                unsigned int inoden = nmap[portdummynode];

                // get voltage
                vj = res(inodep - 1, 0);

                // get current
                ij = (res(inoden - 1, 0) - res(inodep - 1, 0)) / Z0; // notice direction of current
            }
            else
            {
                vj = res(inodep - 1, 0);
                ij = -res(inodep - 1, 0) / Z0; // notice direction of current
            }

            return vj - ij * Z0;
        }
        case hport:
        {
            if(row == 0)
            {
                return res(nmap[ports[row].get_nodes()[0]] - 1, 0);
            }
            else
            {
                return -res(res.rows() - 1, 0);
            }
        }
        case gport:
        {
            if(row == 0)
            {
                return -res(res.rows() - 1, 0);
            }
            else
            {
                return res(nmap[ports[row].get_nodes()[0]] - 1, 0);
            }
        }
        default: // dummy
            return GiNaC::ex();
    }
}

GiNaC::ex get_nport_denominator(port_mode mode, unsigned int row, unsigned int col, const componentlist& components, const GiNaC::matrix& res, nodemap& nmap)
{
    std::vector<component> ports = components.get_components_by_type(ct_port);
    (void) row;
    switch(mode)
    {
        case zport:
            return -res(res.rows() - 1, 0);
        case yport:
            return get_port_voltage(ports[col], res, nmap);
        case sport:
        {
            component p = ports[row];
            unsigned int inodep = nmap[p.get_nodes()[0]];
            unsigned int inoden = nmap[portdummynode];
            GiNaC::possymbol Z0 = get_symbol("Z0");

            // get voltage
            GiNaC::ex vi = res(inodep - 1, 0);

            // get current
            GiNaC::ex ii = (res(inoden - 1, 0) - res(inodep - 1, 0)) / Z0; // notice direction of current

            return vi + ii * Z0;
        }
        case hport:
            return get_symbol("PORTDUMMY");
        case gport:
            return get_symbol("PORTDUMMY");
        default: // dummy
            return GiNaC::ex();
    }
}

void insert_active_port(componentlist& components_tmp, port_mode mode, unsigned int col, const std::vector<component>& ports)
{
    component p = ports[col];
    std::vector<component> for_insertion;
    switch(mode)
    {
        case zport:
            p.set_type(ct_voltage_source);
            for_insertion.push_back(p);
            break;
        case yport:
            p.set_type(ct_current_source);
            for_insertion.push_back(p);
            break;
        case sport:
        {
            component pres = p;

            auto nodes = p.get_nodes();
            std::string nodep = nodes[0];
            std::string noden = nodes[1];
            std::string dummynode(portdummynode);

            nodes[0] = dummynode;
            nodes[1] = noden;
            p.set_nodes(nodes);
            p.set_type(ct_voltage_source);
            for_insertion.push_back(p);

            nodes[0] = nodep;
            nodes[1] = dummynode;
            pres.set_nodes(nodes);
            pres.set_type(ct_resistor);
            pres.set_value(get_symbol("Z0"));
            for_insertion.push_back(pres);
            break;
        }
        case hport:
            p.set_type(col == 0 ? ct_current_source : ct_voltage_source);
            for_insertion.push_back(p);
            break;
        case gport:
            p.set_type(col == 0 ? ct_voltage_source : ct_current_source);
            for_insertion.push_back(p);
            break;
        default: // dummy
            break;
    }
    for(auto& ci : for_insertion)
    {
        components_tmp.add(ci);
    }
}

void insert_inactive_port(componentlist& components_tmp, port_mode mode, unsigned int row, const std::vector<component>& ports)
{
    component p = ports[row];
    p.set_value(0);
    switch(mode)
    {
        case zport:
            p.set_type(ct_current_source);
            break;
        case yport:
            p.set_type(ct_voltage_source);
            break;
        case sport:
            p.set_type(ct_resistor);
            p.set_value(get_symbol("Z0"));
            break;
        case hport:
            p.set_type(row == 0 ? ct_current_source : ct_voltage_source);
            break;
        case gport:
            p.set_type(row == 0 ? ct_voltage_source : ct_current_source);
            break;
        default: // dummy
            break;
    }
    components_tmp.add(p);
}

GiNaC::matrix solve_nport_single(port_mode mode, const componentlist& components, nodemap& nmap, bool linearize, bool print)
{
    std::vector<component> ports = components.get_components_by_type(ct_port);
    GiNaC::matrix port_matrix(ports.size(), ports.size());
    for(unsigned int col = 0; col < ports.size(); ++col)
    {
        componentlist components_tmp(components);
        insert_active_port(components_tmp, mode, col, ports);
        for(unsigned int row = 0; row < ports.size(); ++row)
        {
            if(row != col)
            {
                insert_inactive_port(components_tmp, mode, row, ports);
            }
        }

        // solve network and calculate parameters
        GiNaC::matrix res = solve_network(components_tmp, nmap, linearize, print);
        for(unsigned int row = 0; row < ports.size(); ++row)
        {
            GiNaC::ex num = get_nport_numerator(mode, row, col, components_tmp, res, nmap);
            GiNaC::ex den = get_nport_denominator(mode, row, col, components_tmp, res, nmap);
            if(!den.is_zero())
            {
                port_matrix(row, col) = num / den;
            }
        }
    }
    return port_matrix;
}

void solve_nport(const componentlist& components, nodemap& nmap, result& results, bool linearize, bool print)
{
    std::vector<std::pair<port_mode, std::string>> matrix_container {
        { zport, "Z" },
        { yport, "Y" },
        { sport, "S" },
        { hport, "H" },
        { gport, "G" }
    };
    for(auto& M : matrix_container)
    {
        GiNaC::matrix nmatrix = solve_nport_single(M.first, components, nmap, linearize, print);

        boost::format fmter = boost::format("%d,%d");
        unsigned int number_of_ports = components.number_of_components(ct_port);
        for(unsigned int row = 0; row < number_of_ports; ++row)
        {
            for(unsigned int col = 0; col < number_of_ports; ++col)
            {
                std::string key = str(fmter % (row + 1) % (col + 1));
                results.add(M.second, key, nmatrix(row, col));
            }
        }
    }
}
