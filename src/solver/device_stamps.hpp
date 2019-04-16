#include "stamp.hpp"
#include "../component.hpp"

stamp get_impedance_stamp(const GiNaC::ex& value);
stamp get_conductance_stamp(const GiNaC::ex& value);
stamp get_voltage_source_stamp();
stamp get_vcvs_stamp(const GiNaC::ex& value);
stamp get_ccvs_stamp(const GiNaC::ex& value);
stamp get_vccs_stamp(const GiNaC::ex& value);
stamp get_cccs_stamp(const GiNaC::ex& value);
stamp get_opamp_stamp();
stamp get_port_stamp();
stamp get_dummy_stamp();
stamp get_stamp(const component& c);
