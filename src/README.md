Voctoknopf software
-------------------

This directory contains the software for various versions of the
Voctoknopf, as well as the Raspberry Pi running the tally light.

The source code is split into three parts:

* The *task* source file contains the actual logic and determines
  which actions should be issued in response to button presses and
  status updates from the server.

* The *device* header and source files define the LEDs, buttons and
  IRQs available on a particular device.

* `main.o` (which waits for buttons presses, lights LEDs, and
  communicates with the Voctocore server via TCP) and `protocol.o`
  (which handles the command protocol) and are shared by all targets.

Target names are of the form `<DEVICE>-<TASK>`.  The following targets
are currently defined:

Target          | Device         | Task
----------------|----------------|---------------------------------------------
`v1.0-tdefault` | Voctoknopf 1.0 | Default layout, with “TAKE” button
`v1.1-imatrix`  | Voctoknopf 1.1 | Matrix layout, buttons have immediate effect
`tallypi-tally` | Tally Pi       | Lights up while a specific source is live


The remainder of this document handles the software for Voctoknopf 1.0
and 1.1.  [See here for more information about the tally software.](Tally.md)

---

The `voctoknopf` executable runs on the Ralink RT5350F SoC.  It is
built using the OpenWRT target toolchain:

```
export PATH=${HOME}/openwrt/staging_dir/host/bin:${HOME}/openwrt/staging_dir/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin:$PATH
export STAGING_DIR=${HOME}/openwrt/staging_dir
make
```

The software maintains two states of the main video mixer: the
preselected state (indicated by green LEDs) and the currently active
state (indicated by red LEDs).  The latter is updated whenever the
state of the video mixer changes; the former is private to the device.
Both states comprise an overall mode (single source, composite source,
or off) and between zero and two source choices depending on that
mode.  Pressing “TAKE” sends the preselected state to the mixer and
updates the preselected state to match the former output.  However,
the current output is not updated; this only happens when the
notification about the change is received from the mixer.

The protocol for selecting the projection source is not documented
yet, so some reverse-engineering is necessary in order to make this
work.


Button | Bank | Selects | Description
-------|------|---------|------------
K1S  | 0 | Mode | stream is blanked (source buttons aren't lit)
K2S  | 0 | Mode | `side-by-side-equal` (2 source buttons are lit)
K3S  | 0 | Mode | `side-by-side-preview` (2 source buttons are lit)
K4S  | 0 | Mode | `picture-in-picture` (2 source buttons are lit)
K5S  | 0 | Mode | `fullscreen` (1 source button is lit)
K6S  | 1 | Projection | source 1
K7S  | 1 | Projection | source 2
K8S  | 1 | Projection | source 3
K9S  | 1 | Projection | spare button
K10S | 1 | Projection | no projection
K11S | 2 | Source B | `grabber`
K12S | 2 | Source B | `cam1`
K13S | 2 | Source B | `cam2`
K14S | 2 | Source B | `cam3`
K15S | 2 | Source B | spare button
K16S | 3 | Source A | `grabber`
K17S | 3 | Source A | `cam1`
K18S | 3 | Source A | `cam2`
K19S | 3 | Source A | `cam3`
K20S | 3 | Source A | spare button
K21S | 3 | “TAKE” button | selection is sent to Voctomix, current output becomes new selection


Voctomix treats the two selected sources, the selected composition
mode, and whether the stream is blanked as four separate informations.
However, in the button layout, “no output signal” is one of the
overall stream states.  This means that while in “no output signal”
state, the user interface doesn't specify which output signal is
not-streamed by Voctomix, and when switching out of this state, the
hardware has to send two commands to select the new output and
re-enable the stream.

It may be desireable to send changes in the selection while in “no
output signal” state immediately to Voctomix.  This would mean that
switching into this state only issues a `set_stream_blank` command
(the previous output has become the new selection, which is now equal
to the state of Voctomix), and switching out of that state only issues
a `set_stream_live` command (as the selection has already been made
the new output in Voctomix).  The drawback of this concept is that
it's unclear what should happen if the Voctomix source is changed by
means of another user interface: propagating the change to the current
selection would break the abstraction of the selection being private
to the user interface; ignoring the change would cause an
inconsistency between the selection and the “preview”; and re-sending
the command would cause a conflict if more than one Voctoknopf is
connected to the server.


### How `voctoknopf` deals with connection loss

When a connection error occurs, `voctoknopf` closes the connection and
communicates this to the operator by turning all front LEDs off.  It
then tries to re-connect to the server, and if the connect fails,
waits a second before the next try.

With a TCP connection, connection loss isn't normally detected for
some time unless the connection is closed by the remote end.  However,
it is important to make the Voctoknopf operator aware of this
situation as soon as possible.  Therefore, `voctoknopf` uses two
mechanisms to detect connection loss:

* It sets the socket option `TCP_USER_TIMEOUT` to 1000, causing the
  TCP connection to be forcibly closed and `read(2)` to return
  `ETIMEDOUT` if transmitted data remains unacknowledged for 1000ms.

* It sets the socket options `SO_KEEPALIVE` and the corresponding
  parameter options `TCP_KEEPIDLE`, `TCP_KEEPINTVL`, and `TCP_KEEPCNT`
  to 1, causing an empty keepalive packet to be sent when the socket
  has been idle for 1 second and then every 1 second after that, and
  the connection to be dropped if 1 keepalive packet hasn't been
  acknowledged when the next one is about to be sent.

This means that a connection loss is usually detected after 1–2
seconds but can take up to 3 seconds to detect in the worst-case
scenario.


### Hooking `voctoknopf` into the OpenWRT boot process

To have the `voctoknopf` binary automatically started when the device
is booted up, copy `voctoknopf.init` to `/etc/init.d/voctoknopf` on
the device and insert the appropriate Voctocore host IP.  Then run
`/etc/init.d/voctoknopf enable` to create the matching symlinks in
`/etc/rc.d/`.

While the `voctoknopf` service is running, its process ID is available
in `/var/run/voctoknopf.pid`, and the messages normally printed to
stderr are redirected to the OpenWRT `logd` where they can be viewed
using
```
# logread -e voctoknopf
```
You can use the `-f` option to observe new messages as they appear.
With the default settings, the process is respawned after 5 seconds if
it should die for any reason (which should not happen in normal
operation).
