#include "symbol.hpp"

#include <map>
#include <string>

const GiNaC::symbol& get_symbol(const std::string& str)
{
    static std::map<std::string, GiNaC::symbol> directory;
    std::map<std::string, GiNaC::symbol>::iterator it = directory.find(str);
    if (it != directory.end())
    {
        return it->second;
    }
    else
    {
        auto ret = directory.insert(std::make_pair(str, GiNaC::symbol(str)));
        return ret.first->second;
    }
}

