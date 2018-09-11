#ifndef RESULT_HPP
#define RESULT_HPP

#include <ginac/ginac.h>
#include <string>
#include <map>

#include "componentlist.hpp"
#include "nodemap.hpp"

class result
{
    public:
        result(const componentlist& components,  const GiNaC::matrix& results, const nodemap& nmap);
        void print(const std::vector<std::string>& print_cmd) const;

        void print_all() const;
        void print_voltage(const std::string& voltage) const;
        void print_voltage(const std::string& voltage1, const std::string& voltage2) const;
        void print_current(const std::string& device) const;

    private:
        std::map<std::string, GiNaC::ex> resultmap;
};

#endif // RESULT_HPP
