#ifndef SPECTRE_COMPONENT_PARSER_HPP
#define SPECTRE_COMPONENT_PARSER_HPP

#include <vector>
#include <string>
#include <map>

#include <ginac/ginac.h>

#include <boost/fusion/include/vector.hpp>

#include "parser_common.hpp"

struct spectre_component_parser_type : public qi::grammar<Iterator, Skipper_type, component()>
{
    spectre_component_parser_type();

    qi::rule<Iterator, std::string()> name, terminal, value;
    qi::rule<Iterator, Skipper_type, std::vector<std::string>()> terminals;
    qi::rule<Iterator, Skipper_type, spectre_component_proxy()> device;
    qi::rule<Iterator, Skipper_type, component()> main;
};

#endif // SPECTRE_COMPONENT_PARSER_HPP
