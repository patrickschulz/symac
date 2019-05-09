#include "noise.hpp"

#include "../symbol.hpp"

#include <boost/format.hpp>

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

componentlist remove_sources(const componentlist& components)
{
    componentlist working;
    for(component c : components)
    {
        if(c.get_type() == ct_voltage_source || c.get_type() == ct_current_source)
        {
            c.set_value(0);
        }
        working.add(c);
    }
    return working;
}

void solve_noise(const componentlist& components, nodemap& nmap, result& results, bool linearize, bool print)
{
    componentlist working = remove_sources(components);

    for(unsigned int node = 0; node < components.number_of_nodes(); ++node)
    {
        GiNaC::ex totalnoise = 0;
        GiNaC::ex totalintegratednoise = 0;
        for(const component& c : working)
        {
            if(c.is_noisy())
            {
                // add noise source
                componentlist components_tmp(working);
                component noisesource = c.get_noise_source();
                components_tmp.add(noisesource);

                // solve network and calculate parameters
                if(print)
                {
                    std::cout << "Node: " << nmap[node + 1] << ", Device: " << c.get_name() << '\n';
                }
                GiNaC::matrix res = solve_network(components_tmp, nmap, linearize, print); 

                // calculate noise
                GiNaC::ex value = res(node, 0);
                GiNaC::ex NTF = value / noisesource.get_value();
                totalnoise += squared_abs(NTF) * noisesource.get_value();
                totalintegratednoise += integrate_NTF_sabs(NTF) * noisesource.get_value();

                // add NTF to results
                boost::format fmter = boost::format("%s,%s");
                std::string key = str(fmter % c.get_name() % nmap[node + 1]);
                results.add("NTF", key, NTF);
            }
        }
        results.add("VN", nmap[node + 1], totalnoise);
        results.add("VNI", nmap[node + 1], totalintegratednoise);
    }

    for(unsigned int refnode = 0; refnode < components.number_of_nodes(); ++refnode)
    {
        for(unsigned int noisenode = 0; noisenode < components.number_of_nodes(); ++noisenode)
        {
            GiNaC::ex noise = results.get("VN", "vout");
            GiNaC::ex numerator = results.get("V", nmap[refnode + 1]);
            GiNaC::ex denominator = results.get("V", "vout");

            if(!denominator.is_zero())
            {
                GiNaC::ex equal = noise * pow(numerator / denominator, 2);
                boost::format fmter = boost::format("%s,%s");
                std::string key = str(fmter % nmap[refnode + 1] % nmap[noisenode + 1]);
                results.add("VNeq", key, equal);
            }
        }
    }
}

