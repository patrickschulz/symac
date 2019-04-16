#ifndef STAMP_HPP
#define STAMP_HPP

#include <map>
#include <string>
#include <ginac/ginac.h>

#include "../symbol.hpp"

class element
{
    public:
        element(unsigned int r, unsigned int c, const GiNaC::ex& v) :
            row(r), column(c), value(v)
        { }
        element(unsigned int r, unsigned int c, const std::string& str) :
            row(r), column(c), value(get_symbol(str))
        { }
        unsigned int get_row() const { return row; };
        unsigned int get_column() const { return column; };
        const GiNaC::ex& get_value() const { return value; };
    private:
        unsigned int row;
        unsigned int column;
        GiNaC::ex value;
};

class stamp
{
    public:
        void write(unsigned int node1, unsigned int node2, const GiNaC::ex& value);
        void clear();
        unsigned int size() const;
        std::vector<element>::const_iterator begin() const;
        std::vector<element>::const_iterator end() const;

    private:
        std::vector<element> elements;
};

#endif // STAMP_HPP
