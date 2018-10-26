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

#include <stdio.h>
#include "voctoknopf.h"

static enum {
	m_off,
	m_sbs_equal,
	m_sbs_preview,
	m_picture_in_picture,
	m_fullscreen,
	m_unknown = -1
} mode_active, mode_presel;

static enum {
	s_projection,
	s_cam1,
	s_cam2,
	s_cam3,
	s_extra,
	s_unknown = -1
} srca_active, srca_presel, srcb_active, srcb_presel;

static enum {
	p_info,
	p_slides,
	p_doccam,
	p_extra,
	p_black,
	p_unknown = -1
} projection;

static enum led LED_MODE_ACTIVE[] = {
	led_k1red,
	led_k2red,
	led_k3red,
	led_k4red,
	led_k5red
};
static enum led LED_MODE_PRESEL[] = {
	led_k1green,
	led_k2green,
	led_k3green,
	led_k4green,
	led_k5green
};

static enum led LED_PROJ[] = {
	led_k6blue,
	led_k7blue,
	led_k8blue,
	led_k9blue,
	led_k10blue
};

static enum led LED_SRCB_ACTIVE[] = {
	led_k11red,
	led_k12red,
	led_k13red,
	led_k14red,
	led_k15red
};
static enum led LED_SRCB_PRESEL[] = {
	led_k11green,
	led_k12green,
	led_k13green,
	led_k14green,
	led_k15green
};

static enum led LED_SRCA_ACTIVE[] = {
	led_k16red,
	led_k17red,
	led_k18red,
	led_k19red,
	led_k20red
};
static enum led LED_SRCA_PRESEL[] = {
	led_k16green,
	led_k17green,
	led_k18green,
	led_k19green,
	led_k20green
};

static const char *source_names_for_server[] = {
	"slides",
	"cam1",
	"cam2",
	"cam3",
	"<undefined>"
};
static const char *mode_names_for_server[] = {
	NULL,
	"side_by_side_equal",
	"side_by_side_preview",
	"picture_in_picture",
	"fullscreen"
};


static void update_green_tally();
static void update_preview();


void init()
{
	mode_active = m_unknown;
	mode_presel = m_unknown;

	srca_active = s_unknown;
	srca_presel = s_unknown;
	srcb_active = s_unknown;
	srcb_presel = s_unknown;

	projection = p_unknown;
}


static void update_leds()
{
	unsigned int i;
	bool used;

	for (i = 0; i < 5; i++)
		set_led(LED_MODE_ACTIVE[i], mode_active == i);
	for (i = 0; i < 5; i++)
		set_led(LED_MODE_PRESEL[i], mode_presel == i);

	used = mode_active != m_unknown && mode_active != m_off;
	for (i = 0; i < 5; i++)
		set_led(LED_SRCA_ACTIVE[i], used && srca_active == i);
	used = mode_presel != m_unknown && mode_presel != m_off;
	for (i = 0; i < 5; i++)
		set_led(LED_SRCA_PRESEL[i], used && srca_presel == i);

	used = mode_active != m_unknown && mode_active != m_off
					&& mode_active != m_fullscreen;
	for (i = 0; i < 5; i++)
		set_led(LED_SRCB_ACTIVE[i], used && srcb_active == i);
	used = mode_presel != m_unknown && mode_presel != m_off
					&& mode_presel != m_fullscreen;
	for (i = 0; i < 5; i++)
		set_led(LED_SRCB_PRESEL[i], used && srcb_presel == i);
}

static void update_projection_leds()
{
	for (unsigned int i = 0; i < 5; i++)
		set_led(LED_PROJ[i], projection == i);
}


void bank0(int button)
{
	mode_presel = button;
	update_leds();
	update_green_tally();
	update_preview();
}

void bank1(int button)
{
	/* not yet implemented */
}

void bank2(int button)
{
	if (button != s_projection && button != s_cam1 && button != s_cam2)
		return;

	srcb_presel = button;
	if (mode_presel == m_off || mode_presel == m_fullscreen)
		if (mode_active == m_off || mode_active == m_fullscreen)
			mode_presel = m_picture_in_picture;
		else
			mode_presel = mode_active;

	update_leds();
	update_green_tally();
	update_preview();
}

void bank3(int button)
{
	if (button != s_projection && button != s_cam1 && button != s_cam2)
		return;

	srca_presel = button;
	if (mode_presel == m_off)
		mode_presel = m_fullscreen;

	update_leds();
	update_green_tally();
	update_preview();
}

void take()
{
	if (mode_presel == m_off) {
		send_cmd("set_stream_blank pause\n");
	} else if (mode_presel == m_fullscreen) {
		char cmd[1024];
		snprintf(cmd, sizeof cmd,
			 "set_videos_and_composite %s * fullscreen\n",
			 source_names_for_server[srca_presel]);
		send_cmd(cmd);
		send_cmd("set_stream_live\n");
	} else {
		char cmd[1024];
		snprintf(cmd, sizeof cmd,
			 "set_videos_and_composite %s %s %s\n",
			 source_names_for_server[srca_presel],
			 source_names_for_server[srcb_presel],
			 mode_names_for_server[mode_presel]);
		send_cmd(cmd);
		send_cmd("set_stream_live\n");
	}

	mode_presel = mode_active;
	srca_presel = srca_active;
	srcb_presel = srcb_active;
	update_green_tally();

	/* don't update XXX_active; wait for server response instead */
}

static void update_green_tally()
{
	if (mode_presel == m_off) {
		send_cmd("message green * * pause\n");
	} else if (mode_presel == m_fullscreen) {
		char cmd[1024];
		snprintf(cmd, sizeof cmd,
			 "message green %s * fullscreen\n",
			 source_names_for_server[srca_presel]);
		send_cmd(cmd);
	} else {
		char cmd[1024];
		snprintf(cmd, sizeof cmd,
			 "message green %s %s %s\n",
			 source_names_for_server[srca_presel],
			 source_names_for_server[srcb_presel],
			 mode_names_for_server[mode_presel]);
		send_cmd(cmd);
	}
}

static void update_preview()
{
	/* While the stream is blanked, send the pre-selected mode
	   immediately to the server (instead of waiting for a TAKE)
	   so it can serve as a "poor man's preview". */

	if (mode_active != m_off)
		return;

	switch (mode_presel) {
		char cmd[1024];
	case m_off:
		/* nothing to do */
		break;
	case m_fullscreen:
		snprintf(cmd, sizeof cmd,
			 "set_videos_and_composite %s * fullscreen\n",
			 source_names_for_server[srca_presel]);
		send_cmd(cmd);
		break;
	default:
		snprintf(cmd, sizeof cmd,
			 "set_videos_and_composite %s %s %s\n",
			 source_names_for_server[srca_presel],
			 source_names_for_server[srcb_presel],
			 mode_names_for_server[mode_presel]);
		send_cmd(cmd);
	}
}


void server_status_changed()
{
	if (server_stream_status == ss_unknown ||
	    server_composite_mode == cm_unknown ||
	    server_video_status_a == vs_unknown ||
	    server_video_status_b == vs_unknown) {
		/* server state not fully known */
		mode_active = m_unknown;
		srca_active = m_unknown;
		srcb_active = m_unknown;
	} else {
		if (server_stream_status != ss_live)
			mode_active = m_off;
		else switch (server_composite_mode) {
		case cm_fullscreen:
			mode_active = m_fullscreen; break;
		case cm_side_by_side_equal:
			mode_active = m_sbs_equal; break;
		case cm_side_by_side_preview:
			mode_active = m_sbs_preview; break;
		case cm_picture_in_picture:
			mode_active = m_picture_in_picture; break;
		}

		switch (server_video_status_a) {
		case vs_cam1: srca_active = s_cam1; break;
		case vs_cam2: srca_active = s_cam2; break;
		case vs_cam3: srca_active = s_cam3; break;
		case vs_slides: srca_active = s_projection; break;
		}
		switch (server_video_status_b) {
		case vs_cam1: srcb_active = s_cam1; break;
		case vs_cam2: srcb_active = s_cam2; break;
		case vs_cam3: srcb_active = s_cam3; break;
		case vs_slides: srcb_active = s_projection; break;
		}

		if (mode_presel == m_unknown || srca_presel == s_unknown ||
						srcb_presel == s_unknown) {
			/* on startup, pre-select currently active state */
			mode_presel = mode_active;
			srca_presel = srca_active;
			srcb_presel = srcb_active;
			update_green_tally();
		}
	}

	update_leds();
}
