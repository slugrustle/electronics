## Big\_Fast\_Switch

This project is a single-board assembly containing the following power electronic devices

- Large, high-voltage film capacitor (B32756G8156K000)
- High-voltage, low Rds(on), wide SOA N-Channel MOSFET (STY145N65M5)
- High-voltage, high-current fast rectifier (VS-90EPF06L-M3)

and the following support electronics / devices

- An isolated gate driver for the N-MOSFET
- Input logic (3.3V level, 5V tolerant) to control the N-MOSFET
- Independent heatsinks, fans, and temperature sensors for the N-MOSFET and Rectifier
- Overtemperature detection and latch circuitry that opens the N-MOSFET if either the N-MOSFET or Rectifier exceeds a temperature limit
- Separate AC->DC power supplies for the gate drive voltage supply (floating) and for the input logic, overtemperature circuitry, and cooling fans

This assembly was designed to aid in generating input voltage transients for the testing of DC->DC power supplies, but it could perhaps find other uses.

WARNING: Dangerous high voltages are present on this board including AC line voltage and high DC voltages. The construction or operation of this device is solely at your own risk. Do not attempt unless you are competent to personally evaluate the safety of high voltage electronic systems.

### Other Info

- Pictures of an assembled unit are available in the root of this repository.
- Design files for the board and schematic (as well as PDF / SVG printouts) are stored in the KiCad directory. I ordered the boards from PCBWay as 4-layer FR-4 boards with 3oz copper on all layers.
- STL files for a PCB holder and mounts for the cooling fans are available in the OpenSCAD directory. I printed these with flame retardant ABS filament.
- A full bill of materials can be found in the Bill\_of\_Materials directory.
- I attempted to save datasheets for all utilized components in the Datasheets directory.
- An LTspice simulation of the overtemperature detection and latch circuitry can be found in the Simulation directory. There are ancillary files from my attempt to create a reasonable model of the APT2012SURCK red LED.
- See LICENSE.txt for licensing info. I have endeavored to clarify which data stored here is my own work and which data belongs to others; all work that is my own is released under the CC0 1.0 Universal Public Domain Dedication.

