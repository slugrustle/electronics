## 2&#x2011;Channel High Input Impedance Unity&#x2011;Gain Buffer Amplifier

This project is a 2&#x2011;channel unity&#x2011;gain buffer amplifier built around the Texas Instruments OPA192. The amplifier inputs have ESD protection and high input impedance. The amplifier will behave normally with up to &#xb1;17.5V input voltage. It runs from a 20VAC 200mA supply such as the Triad Magnetics WAU20&#x2011;200 or similar.

There is a 120&#x3a9; impedance inside the amplifier feedback loop after the internal opamp output pin which limits the output voltage range at heavier output loads. Due to this phenomenon and the thermal characteristics of the voltage regulators, it is recommended that the impedance of the load applied to the output of each amplifier be 1.1k&#x3a9; or higher.

To maintain high input impedance, there is no resistance biasing the external amplifier inputs to GND in a default state. Consequently, if one channel is unused, it is recommended to connect IN to COM (or GND) for that channel.
