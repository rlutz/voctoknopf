Tally light
-----------

The tally light consists of a Raspberry Pi connected to a multi-color
LED via GPIO pins and to the Voctomix core via Ethernet.  The software
runs on the Pi and controls the LED, lighting it up while the
corresponding camera source is live.  If, for some reason, a
connection to the core can't be achieved, an error condition is
signalled instead.


### Installation

Raspbian has all necessary tools installed by default, so you can just
build the software on the Pi itself.  To do so, clone or copy this
directory to the Pi and change into it.  From there, do:
```
make all-tally
sudo make install-tally
```
Then, configure the appropriate Voctocore host IP and source in
`/etc/tally.conf` and reboot the device.

To uninstall, do:
```
sudo make uninstall-tally
```


### Device tree overlay

The hardware attached to the Pi is described by the device tree
overlay `tallypi-overlay.dts`.  This source file is compiled into its
binary representation `tallypi.dtbo`, which in turn is copied to
`/boot/overlays/` and instantiated in `/boot/config.txt` in order to
become active.  After a reboot, the three colors of the LED are
available as individual LEDs in `/sys/class/leds/`.

If you want to change the GPIO pins to which the LEDs are connected,
you need to pass the new GPIO numbers as parameters to the device tree
overlay by adding them to the instantiating line in
`/boot/config.txt`, for example:

```
dtoverlay=tallypi,live_gpio=22,preview_gpio=23
```

The following configuration options are available:

* `live_gpio`, `preview_gpio`, `status_gpio`: Set the GPIO (BCM)
  number of the live, preview, and status LED, respectively.  This is
  *not* the pin number on the header; see https://pinout.xyz/ for
  details.

* `live_activelow`, `preview_activelow`, `status_activelow`: Set
  whether the corresponding LED is active low.  A value of `1` means
  that the pin should be high on bootup and changed to low by the
  software; `0` means that the pin should be low on bootup and changed
  to high by the software.


### Automated starting as a systemd service

The tally software is described to systemd via the service file
`/lib/systemd/system/tally.service` and is hooked into or removed from
the boot process using
```
sudo systemctl enable tally
sudo systemctl disable tally
```
You can start or stop it manually with
```
sudo systemctl start tally
sudo systemctl stop tally
```
The messages normally printed to stderr are redirected to the systemd
journal where they can be viewed using
```
journalctl -u tally
```
(use `-f` to observe new messages as they appear).

With the default settings, the process is respawned after 5 seconds if
it should die for any reason (which should not happen in normal
operation).
