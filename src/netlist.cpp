#include "netlist.hpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <regex>
#include <ginac/ginac.h>

#include "util.hpp"
#include "symbol.hpp"
#include "parser/netlist_parser.hpp"

void netlist::add_component(const component& c)
{
    components.add_component(c);
}

void netlist::add_command(const command& c)
{
    commands.push_back(c);
}

struct netlist_printer_type : public boost::static_visitor<>
{
    netlist_printer_type(netlist& nlist) :
        nlist(nlist)
    { }

    void operator() (const component& c) const
    {
        nlist.add_component(c);
    }

    void operator() (const command& c) const
    {
        nlist.add_command(c);
    }

    void operator() (const subcircuit_proxy& s)
    {
        subcircuits[s.name] = s;
    }

    void operator() (const subcircuit_instance_proxy& inst)
    {
        subcircuit_proxy proxy = subcircuits[inst.name];
        subcircuit sub = subcircuit(proxy.name, proxy.terminals);
        for(const component& c : proxy.components)
        {
            sub.add_component(c);
        }
        std::vector<component> subcomponents = sub.get_mapped_components(inst.terminals);
        for(component& c : subcomponents)
        {
            std::string instname = "sub";
            c.name_prepend(instname + "/");
            nlist.add_component(c);
        }
    }

    void operator() (const comment& c) const
    {
        (void)c;
    }

    netlist& nlist;
    std::map<std::string, subcircuit_proxy> subcircuits;
};

void netlist::read(const std::string& filename)
{
    std::ifstream file(filename);
    if(!file.is_open())
    {
        valid = false;
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string str = buffer.str();
    auto iter = str.begin();
    std::vector<netlist_attribute_type> lines;
    bool r = qi::phrase_parse(iter, str.end(), netlist_parser, qi::blank, lines);

    netlist_printer_type visitor(*this);

    if (r && iter == str.end())
    {
        std::cout << "parse succeeded\n";
        for(auto e : lines)
        {
            apply_visitor(visitor, e);
        }
    }
    else
    {
        std::cout << "parse failed. Remaining unparsed: " << std::string(iter, str.end()) << '\n';
    }

    valid = true;
}

netlist::operator bool()
{
    return valid;
}

