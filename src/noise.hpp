#ifndef NOISE_HPP
#define NOISE_HPP

#include <ginac/ginac.h>

#include "transfer_function/transfer_function.hpp"

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

#endif // NOISE_HPP
