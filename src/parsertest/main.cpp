#include <string>
#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

namespace qi = boost::spirit::qi;
namespace ph = boost::phoenix;

using namespace qi::labels;

int main()
{
    std::vector<std::string> strs {
        "A 1",
        "A B"
    };

    qi::rule<std::string::iterator, std::string(), qi::space_type> parser = qi::alpha > qi::alpha;
    parser.name("main");

    qi::on_error<qi::fail>
    (
        parser,
        std::cout
            << ph::val("Error! Expecting ")
            << _4                               // what failed?
            << ph::val(" here: \"")
            << ph::construct<std::string>(_3, _2)   // iterators to error-pos, end
            << ph::val("\"\n")
    );

    for(auto& str : strs)
    {
        std::string result;
        bool r = qi::phrase_parse(str.begin(), str.end(), parser, qi::space, result);
        
        if(r)
        {
            std::cout << str << ": success (" << result << ")\n";
        }
        else
        {
            std::cout << str << ": failure\n";
        }
    }
}
