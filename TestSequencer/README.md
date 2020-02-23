## Test Sequencer

This project surrounds an 8&#x2011;bit PIC microcontroller (PIC16F15324) with

1. Power supply from a DC barrel jack with both ESD and reverse polarity protection. A 5V linear regulator runs the MCU and output buffers.
2. A 32MHz external oscillator
3. A 6p6c modular jack with ESD protection for ICSP programming with an ICD3 or ICD4
4. 7 32mA capable logic&#x2011;level output buffers broken out to terminal blocks with ESD protection
5. An input for a toggle switch or pushbutton broken out to a terminal block with ESD protection
6. A slightly hacky but functional enclosure

After many times wanting to write some very basic assembly code to debounce a button and generate some simple logic&#x2011;level waveforms
to control other equipment or directly interface with a circuit under test, I decided to build a board to serve this role.

The PIC16F15324 was chosen primarily out of familiarity but also for its reasonable specs and capability (relative to this application) and for having no serious errata.
