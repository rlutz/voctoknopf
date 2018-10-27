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

#include "voctoknopf.h"

static const enum server_video_status us = vs_cam1;

static bool red, green;
static void update_leds();


void init()
{
	red = green = false;
	update_leds();
}

int blink()
{
	return -1;
}

void button(int i)
{
	/* tally doesn't have buttons */
}

static void update_leds()
{
	set_led(led_red, red);
	set_led(led_green, green && !red);
}

void server_status_changed()
{
	if (server_stream_status != ss_live ||
	    server_composite_mode == cm_unknown)
		red = false;
	else if (server_composite_mode == cm_fullscreen)
		red = server_video_status_a == us;
	else
		red = server_video_status_a == us ||
		      server_video_status_b == us;

	update_leds();
}

void green_status_changed()
{
	if (green_composite_mode == cm_unknown)
		green = false;
	else if (green_composite_mode == cm_fullscreen)
		green = green_video_status_a == us;
	else
		green = green_video_status_a == us ||
			green_video_status_b == us;

	update_leds();
}
