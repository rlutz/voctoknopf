Voctoknopf software
-------------------

The `voctoknopf` executable runs on the Ralink RT5350F SoC.  It is
built using the OpenWRT target toolchain:

```
export PATH=${HOME}/openwrt/staging_dir/host/bin:${HOME}/openwrt/staging_dir/toolchain-mipsel_24kc_gcc-7.3.0_musl/bin:$PATH
export STAGING_DIR=${HOME}/openwrt/staging_dir
make
```

The source code is split into two parts:

* `main.c` waits for buttons presses, lights LEDs, and communicates
  with the Voctocore server via TCP

* `handler.c` contains the actual logic and determines which actions
  should be issued in response to button presses and status updates


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
