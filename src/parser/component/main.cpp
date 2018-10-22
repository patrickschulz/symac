#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/spirit/include/qi.hpp>    
namespace qi = boost::spirit::qi;

struct parser_type : public qi::grammar<std::string::iterator, std::string()>
{
    typedef std::string::iterator Iterator;

    parser_type() : parser_type::base_type(main, "component")
    {
        using qi::alnum;
        using qi::char_;
        using qi::repeat;
        using qi::eol;

        terminal = +(alnum | char_("-:_!"));
        value    = +(char_ - eol);

        two_terminal_device   = char_("RCLVI") >> +qi::blank >> terminal >> +qi::blank >> terminal >> +qi::blank >> value;
        three_terminal_device = char_("O")     >> repeat(3)[terminal];
        four_terminal_device  = char_("EFGH")  >> repeat(4)[terminal] >> value;

        main = two_terminal_device | 
                three_terminal_device | 
                four_terminal_device;
    }

    qi::rule<Iterator, std::string()> terminal, value;
    qi::rule<Iterator, std::string()> two_terminal_device, three_terminal_device, four_terminal_device;
    qi::rule<Iterator, std::string()> main;
} parser;

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        std::exit(1);
    }
    std::string str = argv[1];
    auto iter = str.begin();
    std::string result;
    bool r = qi::parse(iter, str.end(), parser, result);

    if (r && iter == str.end())
    {
        std::cout << "parse succeeded\n";
        std::cout << result << '\n';
    }
    else
    {
        std::cout << "parse failed. Remaining unparsed: " << std::string(iter, str.end()) << '\n';
    }
}

/*
class component
{
    public:
        friend std::ostream& operator<<(std::ostream& stream, const component& c);

        void set_type(char ct)
        {
            type = ct;
        }

        void set_nodes(const std::vector<std::string>& n)
        {
            nodes = n;
        }

        void set_value(const std::string& v)
        {
            value = v;
        }

        char get_type() const
        {
            return type;
        }

        const std::vector<std::string>& get_nodes() const
        {
            return nodes;
        }

        const std::string& get_value() const
        {
            return value;
        }

    private:
        char type;
        std::vector<std::string> nodes;
        std::string value;
};

std::ostream& operator<<(std::ostream& stream, const component& c)
{
    stream << c.type << " (" << c.value << ") { ";
    for(auto n : c.nodes)
    {
        stream << n << ' ';
    }
    stream << '}';
    return stream;
}

BOOST_FUSION_ADAPT_ADT(
    component,
    (char, char, obj.get_type(), obj.set_type(val))
    (const std::vector<std::string>&, const std::vector<std::string>&, obj.get_nodes(), obj.set_nodes(val))
    (std::string, const std::string&, obj.get_value(), obj.set_value(val))
)

struct component_parser_type : public qi::grammar<std::string::iterator, qi::ascii::blank_type, component()>
{
    typedef std::string::iterator Iterator;

    component_parser_type() : component_parser_type::base_type(main, "component")
    {
        using qi::alnum;
        using qi::char_;
        using qi::repeat;
        using qi::eol;

        terminal = +(alnum | char_("-:_!"));
        value    = +(char_ - eol);

        two_terminal_device   = char_("RCLVI") >> repeat(2)[terminal] >> value;
        three_terminal_device = char_("O")     >> repeat(3)[terminal] >> value;
        four_terminal_device  = char_("EFGH")  >> repeat(4)[terminal] >> value;

        main = two_terminal_device | 
                three_terminal_device | 
                four_terminal_device;
    }

    qi::rule<Iterator, std::string()> terminal, value;
    qi::rule<Iterator, component()> two_terminal_device, three_terminal_device, four_terminal_device, main;
} component_parser;
*/
