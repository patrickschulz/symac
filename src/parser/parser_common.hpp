#ifndef PARSER_COMMON_HPP
#define PARSER_COMMON_HPP

#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

typedef std::string::iterator Iterator;
typedef qi::blank_type Skipper_type;

#include "../component.hpp"

BOOST_FUSION_ADAPT_STRUCT(
    parameter_t,
    (std::string, key)
    (std::string, value)
)
BOOST_FUSION_ADAPT_STRUCT(
    spice_component_proxy,
    (component_types, type)
    (std::string, name)
    (std::vector<std::string>, nodes)
    (std::string, value)
    (std::vector<parameter_t>, parameters)
)
BOOST_FUSION_ADAPT_STRUCT(
    spectre_component_proxy,
    (std::string, name)
    (std::vector<std::string>, nodes)
    (component_types, type)
    (std::string, value)
)

#endif // PARSER_COMMON_HPP
