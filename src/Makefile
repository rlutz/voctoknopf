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

TARGET_PREFIX ?= mipsel-openwrt-linux-
OBJECTS = main.o protocol.o v1.0.o handler.o

all: voctoknopf

voctoknopf: $(OBJECTS)
	$(TARGET_PREFIX)gcc -o $@ $^

%.o: %.c voctoknopf.h v1.0.h
	$(TARGET_PREFIX)gcc -c $<

clean:
	rm -f $(OBJECTS) voctoknopf

.PHONY: all clean
