#include "simplification.hpp"

transfer_function simplify(const transfer_function& tf, const std::map<GiNaC::symbol, int, GiNaC::ex_is_less>& weightmap)
{
    transfer_function simple_tf;

    std::vector<polynom> polys {
        tf.get_numerator(),
        tf.get_denominator(),
    };
    for(polynom& p : polys)
    {
        unsigned int degree = p.degree();
        for(unsigned int i = 0; i <= degree; ++i)
        {
            monom m = p.get_monom(i);
            if(m.valid) 
            {
                auto weights = calculate_weights(m.sum_, weightmap);
                auto indices = calculate_indices_to_keep(weights);
                sum s = create_new_expression(m.sum_, indices);
                m.sum_ = s;
            }
            p.set_monom(m, i);
        }
    }
    // simplify prefix
    auto weights = calculate_weights(tf.get_prefix(), weightmap);
    auto indices = calculate_indices_to_keep(weights);
    sum prefix = create_new_expression(tf.get_prefix(), indices);

    simple_tf.set_numerator(polys[0]);
    simple_tf.set_denominator(polys[1]);
    simple_tf.set_prefix(prefix);

    return simple_tf;
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
