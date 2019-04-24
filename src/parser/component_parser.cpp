#include "component_parser.hpp"

#include "parameter_parser.hpp"

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

namespace phoenix = boost::phoenix;

struct ReportError
{
    template<typename Iter>
    void operator()(Iter first_iter, Iter last_iter, Iter error_iter, const qi::info& what) const 
    {
        Iter start = error_iter;
        while(start != first_iter)
        {
            --start;
            if(*start == '\n')
            {
                ++start;
                break;
            }
        }
        Iter end = error_iter - 1;
        while(end != last_iter)
        {
            ++end;
            if(*end == '\n')
            {
                break;
            }
        }
        std::string line(start, end);

        std::cerr << "Parsing error in " << what << '\n';
        std::cerr << line << '\n';
        std::cerr << std::setw(error_iter - start + 1) << '^' << '\n';
    }
};

const phoenix::function<ReportError> report_error = ReportError();

struct two_terminal_identifier_type : qi::symbols<char, component_types>
{
    two_terminal_identifier_type()
    {
        add
            ("R", ct_resistor)
            ("C", ct_capacitor)
            ("L", ct_inductor)
            ("V", ct_voltage_source)
            ("I", ct_current_source)
        ;
    }
} two_terminal_identifier;

struct three_terminal_identifier_type : qi::symbols<char, component_types>
{
    three_terminal_identifier_type()
    {
        add
            ("O", ct_opamp)
            ("M", ct_mosfet)
        ;
    }
} three_terminal_identifier;

struct four_terminal_identifier_type : qi::symbols<char, component_types>
{
    four_terminal_identifier_type()
    {
        add
            ("E", ct_voltage_controlled_voltage_source)
            ("F", ct_current_controlled_voltage_source)
            ("G", ct_voltage_controlled_current_source)
            ("H", ct_current_controlled_current_source)
        ;
    }
} four_terminal_identifier;

struct port_identifier_type : qi::symbols<char, component_types>
{
    port_identifier_type()
    {
        add
            ("P", ct_port)
        ;
    }
} port_identifier;

struct spice_identifier_type : qi::symbols<char, component_types>
{
    spice_identifier_type()
    {
        add
            ("R", ct_resistor)
            ("C", ct_capacitor)
            ("L", ct_inductor)
            ("V", ct_voltage_source)
            ("I", ct_current_source)
            ("O", ct_opamp)
            ("E", ct_voltage_controlled_voltage_source)
            ("F", ct_current_controlled_voltage_source)
            ("G", ct_voltage_controlled_current_source)
            ("H", ct_current_controlled_current_source)
            ("P", ct_port)
        ;
    }
} spice_identifier;

component_parser_type::component_parser_type() : component_parser_type::base_type(main, "component")
{
    using qi::attr;
    using qi::lit;
    using qi::alpha;
    using qi::alnum;
    using qi::char_;
    using qi::repeat;
    using qi::eol;
    using qi::_r1;
    using qi::no_skip;

    name = +alnum;
    terminal = +(alnum | char_("-:_!"));
    value    = +alnum | ('{' >> +(char_ - '}') >> '}');
    parameters = *parameter_parser;
    terminals = repeat(_r1)[terminal];

    two_terminal_device   = two_terminal_identifier   >> no_skip[name] >> terminals(2) >> value >> parameters;
    three_terminal_device = three_terminal_identifier >> no_skip[name] >> terminals(3) >> attr("OPDUMMY") >> parameters;
    four_terminal_device  = four_terminal_identifier  >> no_skip[name] >> terminals(4) >> value >> parameters;
    port                  = port_identifier           >> no_skip[name] >> terminals(2) >> attr("PORTDUMMY") >> parameters;

    main = two_terminal_device   |
           three_terminal_device |
           four_terminal_device  |
           port                  ;

    two_terminal_device.name("two terminal device");
    three_terminal_device.name("three terminal device");
    four_terminal_device.name("four terminal device");

    name.name("name");
    terminals.name("terminals");
    value.name("value");

    //qi::on_error<qi::fail>(two_terminal_device, report_error(qi::_1, qi::_2, qi::_3, qi::_4));
}

component_parser_type component_parser;
