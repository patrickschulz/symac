#include "parameter_parser.hpp"

parameter_parser_type::parameter_parser_type() : parameter_parser_type::base_type(main, "parameter")
{
    using qi::alpha;
    using qi::lit;
    using qi::alnum;

    key = +alpha;
    value = +alnum;
    main = key >> lit("=") >> value;
}

parameter_parser_type parameter_parser;
