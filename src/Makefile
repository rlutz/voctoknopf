# Voctoknopf - video mixer control device for Chaos conferences
# Copyright (C) 2018 Roland Lutz
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

DEVICES = v1.0 v1.1 tallypi
TARGETS = v1.0-tdefault v1.1-imatrix tallypi-tally tallypi.dtbo
COMMON_OBJECTS = main.o protocol.o

v1_0_TARGET_PREFIX = mipsel-openwrt-linux-
v1_1_TARGET_PREFIX = mipsel-openwrt-linux-
tallypi_TARGET_PREFIX = arm-linux-gnueabihf-

prefix = /usr/local

all: $(TARGETS)

$(foreach device,$(DEVICES),$(eval \
$$(filter $(device)-%,$$(TARGETS)): \
$(device)-%: $$(addprefix $(device)-,$$(COMMON_OBJECTS) $(device).o %.o); \
	$$($(subst .,_,$(device))_TARGET_PREFIX)gcc -o $$@ $$^))

$(foreach device,$(DEVICES),$(eval \
$(device)-%.o: %.c voctoknopf.h $(device).h; \
	$$($(subst .,_,$(device))_TARGET_PREFIX)gcc \
		-DDEVICE_H=\"$(device).h\" -c -o $$@ $$<))

tallypi.dtbo: tallypi-overlay.dts
	dtc -W no_unit_address_vs_reg -I dts -O dtb -o $@ $<

clean:
	rm -f *.o $(TARGETS)

all-tally: tallypi-tally tallypi.dtbo

install-tally: all-tally
	install -m 0644 tallypi.dtbo $(DESTDIR)/boot/overlays/
	sed -e '0,/^#\?dtoverlay=tallypi/s/^#\(dtoverlay=tallypi\)/\1/' \
		-i $(DESTDIR)/boot/config.txt
	grep '^dtoverlay=tallypi' $(DESTDIR)/boot/config.txt >/dev/null || \
		echo dtoverlay=tallypi >> $(DESTDIR)/boot/config.txt

	install -d $(DESTDIR)$(prefix)/bin/
	install -m 0755 tallypi-tally $(DESTDIR)$(prefix)/bin/
	test -f $(DESTDIR)/etc/tally.conf || \
		install -m 0644 tally.conf $(DESTDIR)/etc/

	install -d $(DESTDIR)$(prefix)/lib/systemd/system/
	sed -e 's|@prefix@|$(prefix)|' tally.service.in > \
		$(DESTDIR)$(prefix)/lib/systemd/system/tally.service
	systemctl enable tally

	@echo "*** Tally has been installed; please edit" \
	      "$(DESTDIR)/etc/tally.conf and reboot ***"

uninstall-tally:
	-systemctl disable tally
	rm -f $(DESTDIR)$(prefix)/lib/systemd/system/tally.service
	rm -f $(DESTDIR)$(prefix)/bin/tallypi-tally

	sed -e 's/^dtoverlay=tallypi/#&/' -i $(DESTDIR)/boot/config.txt
	rm -f $(DESTDIR)/boot/overlays/tallypi.dtbo

.PHONY: all clean all-tally install-tally uninstall-tally
