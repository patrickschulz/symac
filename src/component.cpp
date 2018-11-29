#include "component.hpp"

#include <sstream>
#include <map>

#include "symbol.hpp"
#include "parser/expression_parser.hpp"

std::map<component_types, std::string> type_map {
    { ct_resistor,                          "R"    },
    { ct_capacitor,                         "C"    },
    { ct_inductor,                          "L"    },
    { ct_voltage_source,                    "V"    },
    { ct_current_source,                    "I"    },
    { ct_opamp,                             "O"    },
    { ct_voltage_controlled_voltage_source, "E"    },
    { ct_current_controlled_voltage_source, "F"    },
    { ct_voltage_controlled_current_source, "G"    },
    { ct_current_controlled_current_source, "H"    },
    { ct_port                             , "P"    }
};

GiNaC::ex convert_symbol(const std::string& s)
{
    if(s.size() > 0 && s.find_first_not_of("0123456789.-") == std::string::npos) // is the string a numeric?
    {
        return std::stod(s);
    }
    else
    {
        return get_symbol(s);
    }
}

GiNaC::ex convert_expression(std::string s)
{
    qi::rule<std::string::iterator, std::string()> identifier = +(qi::char_ - qi::blank);
    symbolic_expression_type symbolic_expression(identifier);

    ast::expression expression;

    bool r = phrase_parse(s.begin(), s.end(), symbolic_expression, qi::blank, expression);
    if (r)
    {
        ast::eval eval(convert_symbol);
        return eval(expression);
    }
    else
    {
        std::cerr << "could not parse expression " << '"' << s << '"' << '\n';
    }
    return GiNaC::ex();
}

component::component(const component_proxy& p) :
    component(p.name, p.type, p.nodes)
{
    value = convert_expression(p.value);
}

component::component(const std::string& name, component_types type, const std::vector<std::string>& nodes) :
    name(name), type(type), nodes(nodes)
{   
    switch(type)
    {
        case ct_resistor:
        case ct_inductor:
            //mna_size = 0; // <---- use for G-matrix-based approach
            //break;
        case ct_capacitor:
        case ct_voltage_source:
        case ct_current_source:
        case ct_voltage_controlled_voltage_source:
        case ct_current_controlled_current_source:
            mna_size = 1;
            terminals.push_back(".p");
            break;
        case ct_opamp:
            mna_size = 1;
            terminals.push_back(".out");
            break;
        case ct_current_controlled_voltage_source:
            mna_size = 2;
            terminals.push_back(".p");
            terminals.push_back(".cp");
            break;
        case ct_voltage_controlled_current_source:
            mna_size = 0;
            break;
        case ct_port: // TODO
            mna_size = 0;
    }
    this->name.insert(0, type_map[type]);
}

std::string component::get_name() const
{
    return name;
}

std::vector<std::string> component::get_terminal_names() const
{
    return terminals;
}

void component::name_prepend(const std::string& prefix)
{
    name = prefix + name;
}

unsigned int component::element_size() const
{
    return mna_size;
}

const component_types& component::get_type() const
{
    return type;
}

const std::vector<std::string>& component::get_nodes() const
{
    return nodes;
}

void component::set_nodes(const std::vector<std::string>& n)
{
    nodes = n;
}

const GiNaC::ex& component::get_value() const
{
    return value;
}

bool component::operator==(component_types ct) const
{
    return type == ct;
}

std::ostream& operator<<(std::ostream& stream, const component& c)
{
    stream << c.name << ' ';
    for(const std::string& n : c.nodes)
    {
        stream << n << ' ';
    }
    stream << c.value;
    return stream;
}

// vim: nowrap
