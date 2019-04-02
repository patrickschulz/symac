#ifndef NOISE_HPP
#define NOISE_HPP

#include <ginac/ginac.h>

#include "transfer_function/transfer_function.hpp"

GiNaC::ex integrate_NTF_sabs(const transfer_function& tf)
{
    GiNaC::ex gain = tf.gain();
    std::vector<GiNaC::ex> poles = tf.poles();
    // solution for one-pole systems
    return gain * gain * abs(poles[0]) / 4;
}

#endif // NOISE_HPP
