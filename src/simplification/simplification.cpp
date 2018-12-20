#include "simplification.hpp"

transfer_function simplify(const transfer_function& tf, const std::map<GiNaC::symbol, int, GiNaC::ex_is_less>& weightmap)
{

}

std::vector<weight_type> calculate_weights(const sum& s, const std::map<GiNaC::symbol, int, GiNaC::ex_is_less>& weightmap)
{
    std::vector<weight_type> weights;
    for(const product& p : s)
    {
        weight_type weight = 0;
        for(const GiNaC::symbol& sym : p)
        {
            auto it = weightmap.find(sym);
            weight += it->second;
        }
        weights.emplace_back(weight);
    }
    return weights;
}

std::vector<index_type> calculate_indices_to_keep(const std::vector<weight_type>& weights)
{
    int maxweight = -1;
    std::vector<index_type> indices;
    for(unsigned int index = 0; index < weights.size(); ++index)
    {
        int weight = weights[index];
        if(weight > maxweight)
        {
            indices.clear();
            indices.push_back(index);
            maxweight = weight;
        }
        else if(weight == maxweight)
        {
            indices.push_back(index);
        }
    }
    return indices;
}

sum create_new_expression(const sum& s, const std::vector<index_type>& indices)
{
    sum newsum;
    index_type index = 0;
    for(const product& p : s)
    {
        if(std::find(indices.begin(), indices.end(), index) != indices.end())
        {
            newsum.add_product(p);
        }
        ++index;
    }
    return newsum;
}
