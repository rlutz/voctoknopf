Voctoknopf board
----------------

**Please note:** The schematics use features recently added to
gEDA/gaf.  Building the netlist and importing the schematics into PCB
require a current development version of the package.

### Design notes

* Unfortunately, 3mm red/green LEDs are only available with a shared
  cathode, which requires using pnp transistors in order to get a
  large enough and predictable current.

* For the Ethernet connection, any of the five Ethernet ports of the
  SOM could be used.  However, since port 4 is configured as uplink
  (which would require more configuration changes in order to allow
  logging in via SSH) and ports 1–3 don't come populated with pull-up
  resistors on the SOM, the obvious choice is port 0.

* Pull-up resistors for SCL/SDA and the Ethernet wires are already
  present on the SOM.

* The blue LEDs start lighting in a barely perceptible way at around
  2.2V.


Version 1.0
-----------

There are two remaining rat lines on the PCB (yellow and green LED);
these are to be connected by wire links.

The tracks connecting the SOM to the Ethernet connector are equal
length, but I haven't considered 100 Ohm and stripline so far.

### Lessons learned

* The pads for the SOT25 and SO-24 packages are too small for
  hand-soldering.

* The electrolytic capacitor next to the SOM is prone to having its
  pads ripped off the PCB when the SOM is inserted or removed.

* The 120Ω series resistors for the blue LEDs are too small; 2.2kΩ
  proved to be a good value.

* It may be a good idea to bundle the LED connectors into one 1x6
  connector to avoid confusion with the power connector.

* The link LED lights up during boot even if there's no link.
