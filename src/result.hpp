#ifndef RESULT_HPP
#define RESULT_HPP

#include <ginac/ginac.h>
#include <string>
#include <map>

#include "componentlist.hpp"
#include "nodemap.hpp"
#include "command.hpp"

class result
{
    public:
        result(const componentlist& components, const GiNaC::matrix& results, const nodemap& nmap);

        void add(const std::string& key, const GiNaC::ex& res);

        void print(const std::vector<command>& print_cmd, bool pretty, bool simpl) const;

    private:
        std::map<std::string, GiNaC::ex> resultmap;
};

#endif // RESULT_HPP
