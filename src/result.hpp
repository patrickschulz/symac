#ifndef RESULT_HPP
#define RESULT_HPP

#include <ginac/ginac.h>

enum resulttype
{
    voltage,
    current
};

class result
{
    public:
        result(const std::string& name);
        int get_index(const GiNaC::matrix&);

    private:
        GiNaC::ex symbol;
};

#endif // RESULT_HPP
