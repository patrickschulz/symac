#ifndef PARSER_COMMON_HPP
#define PARSER_COMMON_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted.hpp>

namespace qi = boost::spirit::qi;

typedef std::string::iterator Iterator;
typedef qi::blank_type Skipper_type;

#include "../component.hpp"

BOOST_FUSION_ADAPT_ADT(
    parameter_t,
    (const std::string&, const std::string&, obj.get_key(), obj.set_key(val))
    (const std::string&, const std::string&, obj.get_value(), obj.set_value(val))
)
BOOST_FUSION_ADAPT_ADT(
    spice_component_proxy,
    (component_types, component_types, obj.get_type(), obj.set_type(val))
    (const std::string&, const std::string&, obj.get_name(), obj.set_name(val))
    (const std::vector<std::string>&, const std::vector<std::string>&, obj.get_nodes(), obj.set_nodes(val))
    (const std::string&, const std::string&, obj.get_value(), obj.set_value(val))
    (const std::vector<parameter_t>&, const std::vector<parameter_t>&, obj.get_parameters(), obj.set_parameters(val))
)
BOOST_FUSION_ADAPT_STRUCT(
    spectre_component_proxy,
    (std::string, name)
    (std::vector<std::string>, nodes)
    (component_types, type)
    (std::string, value)
)

#endif // PARSER_COMMON_HPP
