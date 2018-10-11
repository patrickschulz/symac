#ifndef SUBCIRCUIT_PARSER_HPP
#define SUBCIRCUIT_PARSER_HPP

#include <boost/fusion/include/adapt_adt.hpp>

#include <string>
#include <ostream>

#include "component_parser.hpp"

namespace qi = boost::spirit::qi;

class subcircuit
{
    public:
        friend std::ostream& operator<<(std::ostream& stream, const subcircuit& sc);

        void set_name(const std::string& n)
        {
            name = n;
        }

        const std::string& get_name() const
        {
            return name;
        }

    private:
        std::string name;
};

std::ostream& operator<<(std::ostream& stream, const subcircuit& sc)
{
    stream << sc.name;
    return stream;
}

BOOST_FUSION_ADAPT_ADT(
    subcircuit,
    (std::string, const std::string&, obj.get_name(), obj.set_name(val))
)

struct subcircuit_parser_type : public qi::grammar<std::string::iterator, qi::ascii::space_type, std::string()>
{
    typedef std::string::iterator Iterator;

    subcircuit_parser_type() : subcircuit_parser_type::base_type(main)
    {
        using qi::alnum;
        using qi::omit;

        name    %= +alnum;
        header  %= ".subckt" >> name;
        end      = ".end";

        main    %= header >> name >> end;
    }

    qi::rule<Iterator> start, end;
    qi::rule<Iterator, qi::ascii::space_type, std::string()> header;
    qi::rule<Iterator, std::string()> name;
    qi::rule<Iterator, qi::ascii::space_type, std::string()> main;
} subcircuit_parser;

#endif // SUBCIRCUIT_PARSER_HPP
