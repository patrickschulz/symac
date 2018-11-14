#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>

enum command_type
{
    com_print,
    com_replace
};

struct command
{
    command_type type;
    std::string content;
};

#endif // COMMAND_HPP
