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
#include DEVICE_H


/* provided by main */

void set_led(enum led led, bool value);
void send_cmd(const char *s);


/* provided by protocol handler */

extern enum server_stream_status {
	ss_live,
	ss_blank_pause,
	ss_blank_nostream,
	ss_unknown = -1
} server_stream_status;

extern enum server_composite_mode {
	cm_fullscreen,
	cm_side_by_side_equal,
	cm_side_by_side_preview,
	cm_picture_in_picture,
	cm_unknown = -1
} server_composite_mode;

extern enum server_video_status {
	vs_cam1,
	vs_cam2,
	vs_cam3,
	vs_slides,
	vs_unknown = -1
} server_video_status_a, server_video_status_b;

void server_status(const char *s);


/* provided by device source file */

extern const char *led_gpio[LED_COUNT];
extern const char *button_gpio[BUTTON_COUNT];
extern const char *irq_gpio[IRQ_COUNT];

extern const char *status_led;


/* provided by task source file */

void init(void);
int blink(void);

void button(int i);

void server_status_changed();


#endif
