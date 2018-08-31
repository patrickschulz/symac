# Symac - A symbolic AC-Domain spice simulator
Symac takes a spice netlist (which currently does not entirely conform with regular spice netlists) and calculates the node voltages and device currents in a symbolic manner. 
This means that instead of numeric results, you will get something like vout = -gm * ro * vin.

Symac is currently under heavy development (which means big changes in some time than changes in short time), there are many features that are worked on, 
such as scattering parameters, noise and simplification.

## Current status
The current version of symac can solve any linear network, that is a network made up of resistors, capacitors, inductors and sources. 
All four of the controlled sources are supported (voltage controlled current and voltage sources, current controlled current and voltage sources) as well as, of course, independent sources. 
Additionally, there is an ideal opamp for convenience.
