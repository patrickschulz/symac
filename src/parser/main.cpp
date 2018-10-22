#define BOOST_SPIRIT_DEBUG

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

#include "netlist_parser.hpp"

namespace qi = boost::spirit::qi;

struct netlist_printer_type : public boost::static_visitor<>
{
    public:
        void operator() (const component& c) const
        {
            std::cout << c << '\n';
        }

        void operator() (const std::string& str) const
        {
            std::cout << '"' << str << '"' << '\n';
        }
} netlist_printer;

class netlist
{
    public:
        void parse(const std::string& filename)
        {
            std::ifstream file(filename);
            std::stringstream buffer;
            buffer << file.rdbuf();

            std::string str = buffer.str();
            auto iter = str.begin();
            bool r = qi::phrase_parse(iter, str.end(), netlist_parser, qi::ascii::blank, lines);

            if (r && iter == str.end())
            {
                std::cout << "parse succeeded\n";
                for(auto e : lines)
                {
                    apply_visitor(netlist_printer, e);
                }
            }
            else
            {
                std::cout << "parse failed. Remaining unparsed: " << std::string(iter, str.end()) << '\n';
            }
        }

    private:
        std::vector<boost::variant<component, std::string>> lines;
};

int main()
{
    netlist nlist;
    nlist.parse("example.spice");
}
