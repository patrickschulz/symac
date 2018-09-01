#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

using namespace std;

struct component
{
    vector<string> nodes { "", "" };
    string value;
    string type;
};

struct subcircuit
{
    string name;
    vector<string> nodes;
    vector<component> components;
};

struct netlist
{
    bool idle = true;
    map<string, subcircuit> subcircuits;
    map<string, int> subinstances;
    subcircuit* current_subcircuit;
    vector<component> components;
};

component read_component(string line)
{
    istringstream stream(line);
    component c;
    stream >> c.type >> c.nodes[0] >> c.nodes[1] >> c.value;
    return c;
}

void read_subcircuit_body(const string& line, netlist& nlist)
{
    if(line.find(".ENDS") != string::npos)
    {
        nlist.idle = true;
    }
    else
    {
        component c = read_component(line);
        nlist.current_subcircuit->components.push_back(c);
    }
}

subcircuit read_subcircuit_header(string line)
{
    istringstream stream(line);
    string temp;
    subcircuit s;
    stream >> temp >> s.name;
    while(stream >> temp)
    {
        s.nodes.push_back(temp);
    }
    return s;
}

vector<component> map_subcircuit_nodes(netlist& nlist, string subname, map<string, string> submap)
{
    subcircuit s = nlist.subcircuits[subname];
    vector<component> components;
    for(unsigned int i = 0; i < s.components.size(); ++i)
    {
        component c = s.components[i];
        vector<string> nodes = c.nodes;
        components.push_back(component{ {}, c.value, c.type });
        for(string node : nodes)
        {
            string mapped_node = node;
            if(node.find("!") == string::npos)
            {
                auto it = submap.find(node);
                if(it != submap.end())
                {
                    mapped_node = submap[node];
                }
                else // local net
                {
                    mapped_node = node + ":" + to_string(nlist.subinstances[subname]);
                }
            }
            components[i].nodes.push_back(mapped_node);
        }
    }
    return components;
}

vector<component> translate_subcircuit(string line, netlist& nlist)
{
    istringstream stream(line);
    string subname;
    char X;
    stream >> X >> subname;
    ++nlist.subinstances[subname];
    subcircuit s = nlist.subcircuits[subname];

    string node;
    map<string, string> submap;
    int i = 0;
    while(stream >> node)
    {
        submap[s.nodes[i]] = node;
        ++i;
    }
    return map_subcircuit_nodes(nlist, subname, submap);
}

int main()
{
    vector<string> netlistlines {
        ".SUBCKT sub n1 n2 n3",
            "R n1 localnet R0",
            "R n3 0! R1",
            "C n2 n3 C0",
        ".ENDS",
        "Xsub vin vout vref",
        "Xsub vin vff vzero",
        "L vin vout L0"
    };

    netlist nlist;

    for(string line : netlistlines)
    {
        if(nlist.idle)
        {
            if(line.find(".SUBCKT") != string::npos)
            {
                subcircuit s = read_subcircuit_header(line);
                nlist.subcircuits[s.name] = s;
                nlist.current_subcircuit = &nlist.subcircuits[s.name];
                nlist.idle = false;
            }
            else if(line.find("X") != string::npos)
            {
                vector<component> cs = translate_subcircuit(line, nlist);
                for(auto c : cs)
                {
                    nlist.components.push_back(c);
                }
            }
            else
            {
                nlist.components.push_back(read_component(line));
            }
        }
        else
        {
            read_subcircuit_body(line, nlist);
        }
    }

    for(component c : nlist.components)
    {
        cout << c.type << ' ' << c.nodes[0] << ' ' << c.nodes[1] << ' ' << c.value << '\n';
    }
}

