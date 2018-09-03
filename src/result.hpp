#ifndef RESULT_HPP
#define RESULT_HPP

#include <ginac/ginac.h>
#include <string>

#include "componentlist.hpp"
#include "nodemap.hpp"

class result
{
    public:
        result(const componentlist& components,  const GiNaC::matrix& results, const nodemap& nmap);
        void print(const std::string& mode) const;

    private:
        const componentlist& components;

        GiNaC::matrix results;

        nodemap nmap;
};

#endif // RESULT_HPP
