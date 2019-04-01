#include "component.hpp"
#include "config.hpp"

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
    { ct_mosfet                           , "M"    },
    { ct_port                             , "P"    }
};

GiNaC::ex convert_expression(std::string s)
{
    qi::rule<Iterator, std::string()> identifier = +qi::alnum;
    symbolic_expression_type<std::string> symbolic_expression(identifier);

    ast::expression<std::string> expression;

    bool r = phrase_parse(s.begin(), s.end(), symbolic_expression, qi::blank, expression);
    if (r)
    {
        auto confsym = [](const std::string& s) -> GiNaC::ex
        {
            if(s.size() > 0 && s.find_first_not_of("0123456789.-") == std::string::npos) // is the string a numeric?
            {
                return std::stod(s);
            }
            else
            {
                return get_symbol(s);
            }
        };
        ast::eval<std::string, GiNaC::ex> eval(confsym);
        return eval(expression);
    }
    else
    {
        std::cerr << "could not parse expression " << '"' << s << '"' << '\n';
    }
    return GiNaC::ex();
}

component::component(const spice_component_proxy& p) :
    component(p.name, p.type, p.nodes)
{
    //std::cout << '"' << p.parameters << '"' << '\n';
    value = convert_expression(p.value);
}

component::component(const spectre_component_proxy& p) :
    component(p.name, p.type, p.nodes)
{
    value = convert_expression(p.value);
}

component::component(const std::string& name, component_types ct, const std::vector<std::string>& nodes) :
    name(name), type(ct), nodes(nodes)
{   
    set_type(ct);
    this->name.insert(0, type_map[type]);
}

void component::set_name(const std::string& s)
{
    name = s;
}

std::string component::get_name() const
{
    return name;
}

void component::name_prepend(const std::string& prefix)
{
    name = prefix + name;
}

unsigned int component::element_size() const
{
    return mna_size;
}

bool component::is_noisy() const
{
    switch(type)
    {
        case ct_resistor:
        case ct_conductor:
            return true;
        case ct_capacitor:
        case ct_inductor:
        case ct_voltage_source:
        case ct_voltage_controlled_voltage_source:
        case ct_current_controlled_current_source:
        case ct_current_controlled_voltage_source:
        case ct_voltage_controlled_current_source:
        case ct_current_source:
        case ct_port:
        case ct_opamp:
        case ct_mosfet: // dummy value, since a mosfet (a nonlinear device) should never make it into the mna algorithm
            return false;
    }
}

GiNaC::ex component::get_noise() const
{
    GiNaC::ex boltzmann = get_symbol("k");
    GiNaC::ex temperature = get_symbol("T");
    return 4 * boltzmann * temperature / value;
}

component_types component::get_type() const
{
    return type;
}

void component::set_type(component_types ct)
{
    type = ct;
    switch(ct)
    {
        case ct_resistor:
        case ct_conductor:
        case ct_capacitor:
        case ct_mosfet: // dummy value, since a mosfet (a nonlinear device) should never make it into the mna algorithm
            mna_size = 0;
            break;
        case ct_inductor:
        case ct_voltage_source:
        case ct_voltage_controlled_voltage_source:
        case ct_current_controlled_current_source:
        case ct_opamp:
            mna_size = 1;
            break;
        case ct_current_controlled_voltage_source:
            mna_size = 2;
            break;
        case ct_voltage_controlled_current_source:
        case ct_current_source:
        case ct_port:
            mna_size = 0;
            break;
    }
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

void component::set_value(const GiNaC::ex& e)
{
    value = e;
}

bool component::operator==(component_types ct) const
{
    return type == ct;
}

bool component::operator==(const component& c) const
{
    return c.name  == name && 
           c.type  == type &&
           c.nodes == nodes &&
           c.value == value;
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

std::vector<component> get_small_signal_model(const component& c)
{
    std::vector<component> ssm;
    switch(c.get_type())
    {
        case ct_mosfet:
        {
            auto nodes = c.get_nodes();
            component gm = component();
            gm.set_name(c.get_name() + "gm");
            gm.set_type(ct_voltage_controlled_current_source);
            gm.set_value(get_symbol("gm"));
            gm.set_nodes({ nodes[2], nodes[1], nodes[0], nodes[2] });
            component rout = component();
            rout.set_name(c.get_name() + "rout");
            rout.set_type(ct_resistor);
            rout.set_value(get_symbol("rds"));
            rout.set_nodes({ nodes[1], nodes[2] });
            ssm.push_back(gm);
            ssm.push_back(rout);
            break;
        }
        case ct_voltage_source:
        {
            component s = c;
            s.set_value(0);
            ssm.push_back(s);
            break;
        }
        case ct_resistor:
            ssm.push_back(c);
            break;
    }
    return ssm;
}

// vim: nowrap
