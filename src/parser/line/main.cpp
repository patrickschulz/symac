#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

struct parser_type : public qi::grammar<std::string::iterator, qi::ascii::blank_type, std::vector<int>()>
{
    typedef std::string::iterator Iterator;

    parser_type() : parser_type::base_type(main)
    {
        element = qi::int_;
        line = element | qi::eps;
        main %= +(line >> qi::eol);
    }

    qi::rule<Iterator, int()> element;
    qi::rule<Iterator, qi::ascii::blank_type, int()> line;
    qi::rule<Iterator, qi::ascii::blank_type, std::vector<int>()> main;
} parser;

int main()
{
    std::ifstream file("input");
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string str = buffer.str();
    auto iter = str.begin();
    std::vector<int> lines;
    bool r = qi::phrase_parse(iter, str.end(), parser, qi::ascii::blank, lines);

    if (r && iter == str.end())
    {
        std::cout << "parse succeeded\n";
        for(auto e : lines)
        {
            std::cout << e << '\n';
        }
    }
    else
    {
        std::cout << "parse failed. Remaining unparsed: " << std::string(iter, str.end()) << '\n';
    }
}
