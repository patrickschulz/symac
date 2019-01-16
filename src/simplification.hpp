#ifndef SIMPLIFICATION_HPP
#define SIMPLIFICATION_HPP

#include <ginac/ginac.h>
#include <string>
#include <map>

#include "componentlist.hpp"
#include "nodemap.hpp"
#include "transfer_function/transfer_function.hpp"

class simplification
{
    public:
        simplification(const std::string& lhs = std::string(), const std::string& rhs = std::string(), const std::string& action = std::string()) :
            lhs(lhs), rhs(rhs), action(action)
        { }

    private:
        std::string lhs, rhs;
        std::string action;
};

typedef unsigned int weight_type;
typedef unsigned int index_type;

transfer_function simplify(const transfer_function& tf, const std::map<GiNaC::symbol, int, GiNaC::ex_is_less>& weightmap);
std::vector<weight_type> calculate_weights(const sum& s, const std::map<GiNaC::symbol, int, GiNaC::ex_is_less>& weightmap);
std::vector<index_type> calculate_indices_to_keep(const std::vector<weight_type>& weights);
sum create_new_expression(const sum& s, const std::vector<index_type>& indices);

#endif // SIMPLIFICATION_HPP
