#include "subcircuit_parser.hpp"

subcircuit_parser_type::subcircuit_parser_type() : 
    subcircuit_parser_type::base_type(main, "subcircuit")
{
    using qi::alnum;
    using qi::alpha;
    using qi::eol;
    using qi::omit;

    name      = alpha >> *alnum;
    terminal  = alpha >> *alnum;
    terminals = +terminal;
    body      = (component_parser | omit[comment_parser]) % eol;

    main      = ".subckt" >> name >> terminals >> eol >> body >> eol >> ".end";
}

subcircuit_instance_parser_type::subcircuit_instance_parser_type() : 
    subcircuit_instance_parser_type::base_type(main, "subcircuit_instance")
{
    using qi::alnum;
    using qi::alpha;
    using qi::char_;
    using qi::lit;

    identifier = lit("X") >> +alnum;
    name = alpha >> *alnum;
    terminal = +(alnum | char_("-:_!"));
    terminals = +terminal;

    main = identifier >> name >> terminals;
}

subcircuit_parser_type subcircuit_parser;
subcircuit_instance_parser_type subcircuit_instance_parser;
