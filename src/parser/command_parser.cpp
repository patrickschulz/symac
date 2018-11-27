#include "command_parser.hpp"

struct command_type_parser_type : qi::symbols<char, command_type>
{
    command_type_parser_type()
    {
        add
            (".print"  , com_print)
            (".printl" , com_printlatex)
            (".replace", com_replace)
        ;
    }
} command_type_parser;

command_parser_type::command_parser_type() : command_parser_type::base_type(main, "command")
{
    using qi::char_;
    using qi::eol;
    using qi::lit;

    identifier = command_type_parser;
    content = +(char_ - eol);
    main = identifier >> content;
}

command_parser_type command_parser;
