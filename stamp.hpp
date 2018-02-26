#ifndef STAMP_HPP
#define STAMP_HPP

#include <map>
#include <string>
#include <ginac/ginac.h>

#include "symbol.hpp"

class stamp
{
    public:
        stamp(int r, int c, const GiNaC::ex& v) :
            row(r), column(c), value(v)
        { }
        stamp(int r, int c, const std::string& str) :
            row(r), column(c), value(get_symbol(str))
        { }
        int get_row() const { return row; };
        int get_column() const { return column; };
        const GiNaC::ex& get_value() const { return value; };
    private:
        int row;
        int column;
        GiNaC::ex value;
};

#endif // STAMP_HPP
