#include "behavioural.hpp"

#include "comment_parser.hpp"

behavioural_parser_type::behavioural_parser_type() : behavioural_parser_type::base_type(main, "behavioural")
{
    using qi::char_;
    using qi::alnum;
    using qi::alpha;
    using qi::eol;
    using qi::omit;

    name      = alpha >> *alnum;
    terminal  = alpha >> *alnum;
    terminals = +terminal;

    function = +qi::alpha;
    symbol = +(qi::alnum | qi::char_(" ,."));
    number = +qi::digit >> -(qi::char_(".") >> +qi::digit);
    identifier = (function >> "(" >> symbol >> ")") | (qi::attr("NUMBER") >> number);
    symbolic_expression.set_identifier(identifier);

    bodyline = symbolic_expression | omit[comment_parser];
    body      = bodyline % eol;

    main      = ".behavioural" >> name >> terminals >> eol >> body >> eol >> ".end";
}
