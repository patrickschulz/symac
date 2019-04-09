#ifndef NOISE_HPP
#define NOISE_HPP

#include <ginac/ginac.h>

#include "common.hpp"
#include "../transfer_function/transfer_function.hpp"

GiNaC::ex integrate_NTF_sabs(const transfer_function& tf)
{
    GiNaC::ex gain = tf.gain();
    std::vector<GiNaC::ex> poles = tf.poles();
    GiNaC::ex res = 0;
    switch(poles.size())
    {
        case 0:
            //std::cerr << "No poles found, can't integrate constant functions. Noise result will be wrong.\n";
            break;
        case 1:
            res = gain * gain * abs(poles[0]) / 4;
            break;
        case 2:
            {
                GiNaC::ex p1 = abs(poles[0]);
                GiNaC::ex p2 = abs(poles[1]);
                res = gain * gain * (p1 * p2 * p2 - p1 * p1 * p2) / (4 * (p2 * p2 - p1 * p1));
                break;
            }
        default:
            //std::cerr << "No implementation for transfer functions with more than two poles. Noise result will be wrong.\n";
            break;
    }
    return res;
}

GiNaC::ex integrate_NTF_sabs(const GiNaC::ex& e)
{
    return integrate_NTF_sabs(transfer_function(e));
}

GiNaC::ex squared_abs(const GiNaC::ex& e)
{
    GiNaC::symbol s = get_complex_symbol("s");
    GiNaC::possymbol w = get_symbol("w");
    GiNaC::ex tmp = e.subs(s == GiNaC::I * w);
    return GiNaC::pow(tmp.real_part(), 2) + GiNaC::pow(tmp.imag_part(), 2);
}

void solve_noise(const componentlist& components, nodemap& nmap, result& results)
{
    for(unsigned int node = 0; node < components.number_of_nodes(); ++node)
    {
        GiNaC::ex totalnoise = 0;
        GiNaC::ex totalintegratednoise = 0;
        for(const component& c : components)
        {
            if(c.is_noisy())
            {
                componentlist components_tmp(components);

                GiNaC::ex noise = c.get_noise();
                component source = c;
                source.set_type(ct_current_source);
                source.set_value(noise);
                components_tmp.add_component(source);

                /* solve network and calculate parameters */
                GiNaC::matrix res = solve_network(components_tmp, nmap, false);

                GiNaC::ex value = res(node, 0);
                GiNaC::ex NTF = value / noise;
                totalnoise += squared_abs(NTF) * noise;
                totalintegratednoise += integrate_NTF_sabs(NTF) * noise;

                // add NTF
                boost::format fmter = boost::format("%s,%s");
                std::string key = str(fmter % c.get_name() % nmap[node + 1]);
                results.add("NTF", key, NTF);
            }
        }
        results.add("VN", nmap[node + 1], totalnoise);
        results.add("VNI", nmap[node + 1], totalintegratednoise);
    }
}

#endif // NOISE_HPP
