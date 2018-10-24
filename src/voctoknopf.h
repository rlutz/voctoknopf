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
#include "v1.0.h"


/* provided by main */

void set_led(enum led led, bool value);
void send_cmd(const char *s);


/* provided by device source file */

extern const char *led_gpio[LED_COUNT];
extern const char *button_gpio[BUTTON_COUNT];
extern const char *irq_gpio[IRQ_COUNT];

extern const char *status_led;


/* provided by handler */

void init(void);

void bank0(int button);
void bank1(int button);
void bank2(int button);
void bank3(int button);
void take(void);

void server_status(const char *s);


#endif
