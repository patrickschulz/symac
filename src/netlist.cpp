#include "netlist.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

#include "symbol.hpp"
#include "parser/netlist_parser.hpp"

void netlist::add_component(const component& c)
{
    components.add(c);
}

void netlist::add_command(const command& c)
{
    commands.push_back(c);
}

struct netlist_processor_type : public boost::static_visitor<>
{
    netlist_processor_type(netlist& nlist) :
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

    void operator() (const behavioural& b) const
    {
        std::cout << b.name << '\n';
        for(auto s : b.terminals)
        {
            std::cout << "  " << s << '\n';
        }
    }

    void operator() (const subcircuit& s)
    {
        subcircuits[s.name] = s;
    }

    void operator() (const subcircuit_instance& inst)
    {
        subcircuit sub = subcircuits[inst.name];
        std::vector<component> subcomponents = get_mapped_components(sub, inst.instance, inst.terminals);
        for(component& c : subcomponents)
        {
            c.name_prepend(inst.instance + "/");
            nlist.add_component(c);
        }
    }

    void operator() (const comment& c) const
    {
        (void)c;
    }

    netlist& nlist;
    std::map<std::string, subcircuit> subcircuits;
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

    netlist_processor_type visitor(*this);

    if (r && iter == str.end()) // parse succeeded
    {
        for(auto e : lines)
        {
            apply_visitor(visitor, e);
        }
    }
    else
    {
        std::cout << "parse failed\n";
    }

    valid = true;
}

netlist::operator bool()
{
    return valid;
}

