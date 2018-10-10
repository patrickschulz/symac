#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "netlist_parser.hpp"

namespace qi = boost::spirit::qi;

struct netlist_printer_type : public boost::static_visitor<>
{
    public:
        template<typename T>
        void operator() (const T& operand) const
        {
            std::cout << operand << '\n';
        }
} netlist_printer;

class netlist
{
    public:
        void parse()
        {

            std::cout << "/////////////////////////////////////////////////////////\n\n";
            std::cout << "\t\tParser Test\n\n";
            std::cout << "/////////////////////////////////////////////////////////\n\n";
            std::cout << "Type a Test String ...or [q or Q] to quit\n\n";

            std::string str;
            while (std::getline(std::cin, str))
            {
                if (str.empty() || str[0] == 'q' || str[0] == 'Q')
                    break;

                auto iter = str.begin();
                bool r = qi::phrase_parse(iter, str.end(), netlist_parser, qi::ascii::space, lines);

                if (r && iter == str.end())
                {
                    std::cout << "-------------------------\n";
                    std::cout << "Parsing succeeded\n";
                    for(auto e : lines)
                    {
                        apply_visitor(netlist_printer, e);
                    }
                    std::cout << "-------------------------\n";
                }
                else
                {
                    std::cout << "-------------------------\n";
                    std::cout << "Parsing failed at " << *iter << "\n";
                    std::cout << "-------------------------\n";
                }
            }
        }

    private:
        std::vector<boost::variant<component, std::string>> lines;
};

int main()
{
    netlist nlist;
    nlist.parse();
}
