#include "symbol.hpp"

#include <map>
#include <string>

const GiNaC::possymbol& get_symbol(const std::string& str)
{
    static std::map<std::string, GiNaC::possymbol> directory;
    std::map<std::string, GiNaC::possymbol>::iterator it = directory.find(str);
    if (it != directory.end())
    {
        return it->second;
    }
    else
    {
        auto ret = directory.insert(std::make_pair(str, GiNaC::possymbol(str)));
        return ret.first->second;
    }
}

