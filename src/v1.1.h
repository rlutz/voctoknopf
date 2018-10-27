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

#ifndef V1_1_H
#define V1_1_H

enum led {
	led_k1green, led_k1red,
	led_k2green, led_k2red,
	led_k3green, led_k3red,
	led_k4green, led_k4red,
	led_k5green, led_k5red,
	led_k6green, led_k6red,
	led_k7green, led_k7red,
	led_k8green, led_k8red,
	led_k9green, led_k9red,
	led_k10green, led_k10red,
	led_k11green, led_k11red,
	led_k12green, led_k12red,
	led_k13green, led_k13red,
	led_k14green, led_k14red,
	led_k15green, led_k15red,
	led_k16green, led_k16red,
	led_k17green, led_k17red,
	led_k18green, led_k18red,
	led_k19green, led_k19red,
	led_k20green, led_k20red,
	LED_COUNT
};

enum button {
	BUTTON_COUNT = 21
};

enum irq {
	IRQ_COUNT = 4
};

#endif
