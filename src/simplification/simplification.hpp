#include "../transfer_function/transfer_function.hpp"
#include <ginac/ginac.h>
#include <map>

typedef unsigned int weight_type;
typedef unsigned int index_type;

std::vector<weight_type> calculate_weights(const sum& s, const std::map<GiNaC::symbol, int, GiNaC::ex_is_less>& weightmap);
std::vector<index_type> calculate_indices_to_keep(const std::vector<weight_type>& weights);
sum create_new_expression(const sum& s, const std::vector<index_type>& indices);
