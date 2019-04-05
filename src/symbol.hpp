#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <ginac/ginac.h>

const GiNaC::possymbol& get_symbol(const std::string& str);
const GiNaC::symbol& get_complex_symbol(const std::string& str);

#endif // SYMBOL_HPP
