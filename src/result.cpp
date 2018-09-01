#include "result.hpp"

#include "symbol.hpp"

result::result(const std::string& name) :
    symbol(get_symbol(name))
{
}

int result::get_index(const GiNaC::matrix& m)
{
    for(unsigned int i = 0; i < m.rows(); ++i)
    {
        if(symbol == m(i, 0))
        {
            return i;
        }
    }
    return -1;
}
