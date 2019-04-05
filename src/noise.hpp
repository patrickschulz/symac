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
            std::cerr << "No poles found, can't integrate constant functions. Noise result will be wrong.\n";
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
            std::cerr << "No implementation for transfer functions with more than two poles. Noise result will be wrong.\n";
            break;
    }
    return res;
}

#endif // NOISE_HPP
