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
	"/sys/class/gpio/k1green",
	"/sys/class/gpio/k1red",
	"/sys/class/gpio/k2green",
	"/sys/class/gpio/k2red",
	"/sys/class/gpio/k3green",
	"/sys/class/gpio/k3red",
	"/sys/class/gpio/k4green",
	"/sys/class/gpio/k4red",
	"/sys/class/gpio/k5green",
	"/sys/class/gpio/k5red",
	"/sys/class/gpio/k6green",
	"/sys/class/gpio/k6red",
	"/sys/class/gpio/k7green",
	"/sys/class/gpio/k7red",
	"/sys/class/gpio/k8green",
	"/sys/class/gpio/k8red",
	"/sys/class/gpio/k9green",
	"/sys/class/gpio/k9red",
	"/sys/class/gpio/k10green",
	"/sys/class/gpio/k10red",
	"/sys/class/gpio/k11green",
	"/sys/class/gpio/k11red",
	"/sys/class/gpio/k12green",
	"/sys/class/gpio/k12red",
	"/sys/class/gpio/k13green",
	"/sys/class/gpio/k13red",
	"/sys/class/gpio/k14green",
	"/sys/class/gpio/k14red",
	"/sys/class/gpio/k15green",
	"/sys/class/gpio/k15red",
	"/sys/class/gpio/k16green",
	"/sys/class/gpio/k16red",
	"/sys/class/gpio/k17green",
	"/sys/class/gpio/k17red",
	"/sys/class/gpio/k18green",
	"/sys/class/gpio/k18red",
	"/sys/class/gpio/k19green",
	"/sys/class/gpio/k19red",
	"/sys/class/gpio/k20green",
	"/sys/class/gpio/k20red"
};

const char *button_gpio[BUTTON_COUNT] = {
	"/sys/class/gpio/k1",
	"/sys/class/gpio/k2",
	"/sys/class/gpio/k3",
	"/sys/class/gpio/k4",
	"/sys/class/gpio/k5",
	"/sys/class/gpio/k6",
	"/sys/class/gpio/k7",
	"/sys/class/gpio/k8",
	"/sys/class/gpio/k9",
	"/sys/class/gpio/k10",
	"/sys/class/gpio/k11",
	"/sys/class/gpio/k12",
	"/sys/class/gpio/k13",
	"/sys/class/gpio/k14",
	"/sys/class/gpio/k15",
	"/sys/class/gpio/k16",
	"/sys/class/gpio/k17",
	"/sys/class/gpio/k18",
	"/sys/class/gpio/k19",
	"/sys/class/gpio/k20",
	"/sys/class/gpio/k21"
};

const char *irq_gpio[IRQ_COUNT] = {
	"/sys/class/gpio/u1irq",
	"/sys/class/gpio/u2irq",
	"/sys/class/gpio/u3irq",
	"/sys/class/gpio/u4irq"
};

const char *status_led = NULL;
