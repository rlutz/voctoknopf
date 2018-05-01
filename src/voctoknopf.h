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

#ifndef VOCTOKNOPF_H
#define VOCTOKNOPF_H

#include <stdbool.h>

/* button handlers */

void bank0(int button);
void bank1(int button);
void bank2(int button);
void bank3(int button);
void take(void);

/* socket handler */

void server_status(const char *s);

/* re-callbacks for main */

enum led {
	led_k1green, led_k1red,
	led_k2green, led_k2red,
	led_k3green, led_k3red,
	led_k4green, led_k4red,
	led_k5green, led_k5red,
	led_k6blue,
	led_k7blue,
	led_k8blue,
	led_k9blue,
	led_k10blue,
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

void set_led(enum led led, bool value);
void send_cmd(const char *s);

#endif