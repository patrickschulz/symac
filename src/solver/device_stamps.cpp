#include "device_stamps.hpp"

#include "../symbol.hpp"

stamp get_impedance_stamp(const GiNaC::ex& value)
{
    stamp stmp;
    // G-Matrix
    stmp.write(1, 1,  1 / value);
    stmp.write(2, 2,  1 / value);
    stmp.write(1, 2, -1 / value);
    stmp.write(2, 1, -1 / value);
    /* Impedance approach
    stmp.write(3, 1, 1);
    stmp.write(2, 3, 1);
    stmp.write(3, 2, -1);
    stmp.write(2, 3, -1);
    stmp.write(3, 3, -value);
    */
    return stmp;
}

stamp get_conductance_stamp(const GiNaC::ex& value)
{
    stamp stmp;
    // G-Matrix
    stmp.write(1, 1,  value);
    stmp.write(2, 2,  value);
    stmp.write(1, 2, -value);
    stmp.write(2, 1, -value);
    /* Admittance-aproach
    stmp.write(3, 1, value);
    stmp.write(1, 3, 1);
    stmp.write(3, 2, -value);
    stmp.write(2, 3, -1);
    stmp.write(3, 3, -1);
    */
    return stmp;
}

stamp get_voltage_source_stamp()
{
    stamp stmp;
    stmp.write(3, 1, 1);
    stmp.write(1, 3, 1);
    stmp.write(3, 2, -1);
    stmp.write(2, 3, -1);
    return stmp;
}

stamp get_vcvs_stamp(const GiNaC::ex& value)
{
    stamp stmp;
    stmp.write(5, 1, 1);
    stmp.write(1, 5, 1);
    stmp.write(5, 2, -1);
    stmp.write(2, 5, -1);
    stmp.write(5, 3, -value);
    stmp.write(5, 4, value);
    return stmp;
}

stamp get_ccvs_stamp(const GiNaC::ex& value)
{
    stamp stmp;
    stmp.write(5, 1,  1);
    stmp.write(1, 6,  1);
    stmp.write(5, 2, -1);
    stmp.write(2, 6, -1);
    stmp.write(6, 3,  1);
    stmp.write(3, 5,  1);
    stmp.write(6, 4, -1);
    stmp.write(4, 5, -1);
    stmp.write(5, 5, -value);
    return stmp;
}

stamp get_vccs_stamp(const GiNaC::ex& value)
{
    stamp stmp;
    stmp.write(1, 3, -value);
    stmp.write(1, 4, +value);
    stmp.write(2, 3, +value);
    stmp.write(2, 4, -value);
    return stmp;
}

stamp get_cccs_stamp(const GiNaC::ex& value)
{
    stamp stmp;
    stmp.write(1, 5, -value);
    stmp.write(2, 5,  value);
    stmp.write(5, 3, 1);
    stmp.write(3, 5, 1);
    stmp.write(5, 4, -1);
    stmp.write(4, 5, -1);
    return stmp;
}

stamp get_opamp_stamp()
{
    stamp stmp;
    stmp.write(4, 1, 1);
    stmp.write(4, 2, -1);
    stmp.write(3, 4, 1);
    //stmp.write(4, 4, -1); // TODO: use this line for four-terminal (differential) opamps?
    return stmp;
}

stamp get_port_stamp()
{
    stamp stmp;
    stmp.write(1, 1, 0);
    stmp.write(2, 1, 0);
    stmp.write(1, 2, 0);
    stmp.write(2, 2, 0);
    return stmp;
}

stamp get_dummy_stamp()
{
    stamp stmp;
    stmp.write(1, 1, 0);
    stmp.write(2, 1, 0);
    stmp.write(1, 2, 0);
    stmp.write(2, 2, 0);
    return stmp;
}

stamp get_stamp(const component& c)
{
    GiNaC::ex value = c.get_value();
    switch(c.get_type())
    {
        case ct_resistor:
            return get_impedance_stamp(value);
        case ct_inductor:
            return get_impedance_stamp(value * get_complex_symbol("s"));
        case ct_conductor:
            return get_conductance_stamp(value);
        case ct_capacitor:
            return get_conductance_stamp(value * get_complex_symbol("s"));
        case ct_voltage_source:
            return get_voltage_source_stamp();
        case ct_opamp:
            return get_opamp_stamp();
        case ct_voltage_controlled_voltage_source:
            return get_vcvs_stamp(value);
        case ct_current_controlled_voltage_source:
            return get_ccvs_stamp(value);
        case ct_voltage_controlled_current_source:
            return get_vccs_stamp(value);
        case ct_current_controlled_current_source:
            return get_cccs_stamp(value);
        case ct_port:
            if(c.get_parameter("v") != "")
            {
                return get_voltage_source_stamp();
            }
            return get_port_stamp();
        default: // includes ct_current_source: 
            return get_dummy_stamp();
    }
}

