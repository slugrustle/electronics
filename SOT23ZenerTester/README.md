## Fixture and Software for Testing SOT23&#x2011;3 Zener Diodes

This project includes a circuit board designed to obtain the voltage / current characteristics
of SOT23&#x2011;3 package Zener diodes when used in conjunction with the following test equipment:

1. A Diolan DLN&#x2011;4ME interface adapter to control the fixture board relays for selecting the Zener diode and pullup resistor to use in testing
2. An ITECH IT6100B series power supply (the IT6123B is a good choice) to bias the Zener diode under test via the selected fixture pullup resistor
3. A Rigol DM3068 digital multimeter to measure the Zener diode cathode voltage when the diode is biased
4. A PC with at least 1 USB2 port (for the Diolan DLN&#x2011;4ME) and at least 2 RS232 ports (for the power supply and DMM)

This project also includes software to control the test equipment, calculate the Zener diode voltage / current characteristics from measurement data, and output the results in a .xlsx workbook.

Many elements of the test setup can be specified in a text&#x2011;based configuration file; examples of these can be found in the FixtureProgram/Release directory. There are two test programs: TestZeners, which obtains a full voltage / current curve, and SingleShot, which measures the Zener diode voltage / current at a single fixture setting after a \~1ms delay. Example output files for each program can be found in the FixtureProgram/Release directory.
