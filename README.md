# SymAC - A symbolic AC-Domain spice simulator
Symac takes a spice netlist (which currently does not entirely conform with regular spice netlists) and calculates the node voltages and device currents in a symbolic manner. 
This means that instead of numeric results, you will get something like vout = -gm * ro * vin.

Symac is currently under heavy development (which means that there will be changes in future that are not backwards compatible), there are many features that are worked on, 
such as scattering parameters, noise and simplification.

(C) 2018 Patrick Schulz

## Current status
The current version of symac can solve any linear network, that is a network made up of resistors, capacitors, inductors and sources. 
All four of the controlled sources are supported (voltage controlled current and voltage sources, current controlled current and voltage sources) as well as, of course, independent sources. 
Additionally, there is an ideal opamp for convenience.

There is a simple results browser which can be used to select individual quantities (node voltages and device currents).

## Roadmap
There are some features that i want to implement, some of which have basic functionality, some are not even close to being implemented.
### Subcircuits
Subcircuits are now working. Currently, however, parameters are not supported, which makes subcircuits less useful. Parameters could introduce a new syntax (param = val), which could also be used for
extra parameters for components (for example for noise)
### Simplification
I want to be able to simplify expressions like 1 + s * (R1 * C1 + R2 * C2) to 1 + s * R1 * C1. For this a basic idea exists which is already implemented. However, currently this works only for special cases
and is not generally applicable. This can be taken further, but this needs restructuring of the way results are saved. This is under active development.
### Noise
Symac should support noise simulation without adding special current and voltage sources, as well as calculations as input-referred noise. This should not be too hard, but there is currently no mechanism 
for this.
### Scattering parameters
I think this should be easy, as well as having any n-port parameters. What this needs is a good results browser, some kind of post-processing calculator. Since I want to build something like this anyway,
this could happen kind of automatically some day :)

I started working on the n-port parameters. Z- and Y-Matrix are working, but i need a unified results browser.
