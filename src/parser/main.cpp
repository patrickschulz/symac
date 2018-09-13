#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace qi = boost::spirit::qi;

int
main()
{
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "\t\tParser Test\n\n";
    std::cout << "/////////////////////////////////////////////////////////\n\n";
    std::cout << "Type a Test String ...or [q or Q] to quit\n\n";

    typedef std::string::const_iterator iterator_type;

    std::string str;
    while (std::getline(std::cin, str))
    {
        if (str.empty() || str[0] == 'q' || str[0] == 'Q')
            break;

        auto parser = qi::lit(".subckt") >> qi::omit[+qi::space] >> +qi::alnum % qi::space;

        std::vector<std::string> result;
        auto iter = str.begin();
        bool r = qi::parse(iter, str.end(), parser, result);

        if (r && iter == str.end())
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing succeeded\n";
            for(auto s : result)
            {
                std::cout << s << '\n';
            }
            //std::cout << "result = " << result << std::endl;
            std::cout << "-------------------------\n";
        }
        else
        {
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "-------------------------\n";
        }
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}
