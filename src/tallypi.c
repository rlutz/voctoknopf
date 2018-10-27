/* Voctoknopf - video mixer control device for Chaos conferences
   Copyright (C) 2018 Roland Lutz

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <stdlib.h>
#include "voctoknopf.h"

const char *led_gpio[LED_COUNT] = {
	"/sys/class/leds/tallypi:red:live",
	"/sys/class/leds/tallypi:green:preview"
};

const char *button_gpio[BUTTON_COUNT] = {
};

const char *irq_gpio[IRQ_COUNT] = {
};

const char *status_led =
	"/sys/class/leds/tallypi:blue:status";
