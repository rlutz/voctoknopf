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
#include <string.h>

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

static enum server_stream_status {
	ss_live,
	ss_blank_pause,
	ss_blank_nostream,
	ss_unknown = -1
} server_stream_status;

static enum server_composite_mode {
	cm_fullscreen,
	cm_side_by_side_equal,
	cm_side_by_side_preview,
	cm_picture_in_picture,
	cm_unknown = -1
} server_composite_mode;

static enum server_video_status {
	vs_cam1,
	vs_cam2,
	vs_cam3,
	vs_slides,
	vs_unknown = -1
} server_video_status_a, server_video_status_b;

static void update_green_tally();


void init()
{
	mode_active = m_unknown;
	mode_presel = m_unknown;

	srca_active = s_unknown;
	srca_presel = s_unknown;
	srcb_active = s_unknown;
	srcb_presel = s_unknown;

	projection = p_unknown;

	server_stream_status = ss_unknown;
	server_composite_mode = cm_unknown;
	server_video_status_a = vs_unknown;
	server_video_status_b = vs_unknown;

	send_cmd("get_stream_status\n"
		 "get_composite_mode\n"
		 "get_video\n");
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


static void update_active()
{
	if (server_stream_status == ss_unknown ||
	    server_composite_mode == cm_unknown ||
	    server_video_status_a == vs_unknown ||
	    server_video_status_b == vs_unknown) {
		/* server state not fully known */
		mode_active = m_unknown;
		srca_active = m_unknown;
		srcb_active = m_unknown;
		return;
	}

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

	update_leds();
}

static void stream_status(const char *arg)
{
	if (strcmp(arg, "live") == 0)
		server_stream_status = ss_live;
	else if (strcmp(arg, "blank pause") == 0)
		server_stream_status = ss_blank_pause;
	else if (strcmp(arg, "blank nostream") == 0)
		server_stream_status = ss_blank_nostream;
	else
		server_stream_status = ss_unknown;

	update_active();
}

static void composite_mode(const char *arg)
{
	if (strcmp(arg, "fullscreen") == 0)
		server_composite_mode = cm_fullscreen;
	else if (strcmp(arg, "side_by_side_equal") == 0)
		server_composite_mode = cm_side_by_side_equal;
	else if (strcmp(arg, "side_by_side_preview") == 0)
		server_composite_mode = cm_side_by_side_preview;
	else if (strcmp(arg, "picture_in_picture") == 0)
		server_composite_mode = cm_picture_in_picture;
	else
		server_composite_mode = cm_unknown;

	update_active();
}

static void video_status(const char *arg_a, const char *arg_b)
{
	if (strcmp(arg_a, "cam1") == 0)
		server_video_status_a = vs_cam1;
	else if (strcmp(arg_a, "cam2") == 0)
		server_video_status_a = vs_cam2;
	else if (strcmp(arg_a, "cam3") == 0)
		server_video_status_a = vs_cam3;
	else if (strcmp(arg_a, "slides") == 0)
		server_video_status_a = vs_slides;
	else
		server_video_status_a = vs_unknown;

	if (strcmp(arg_b, "cam1") == 0)
		server_video_status_b = vs_cam1;
	else if (strcmp(arg_b, "cam2") == 0)
		server_video_status_b = vs_cam2;
	else if (strcmp(arg_b, "cam3") == 0)
		server_video_status_b = vs_cam3;
	else if (strcmp(arg_b, "slides") == 0)
		server_video_status_b = vs_slides;
	else
		server_video_status_b = vs_unknown;

	update_active();
}

static void message(const char *arg)
{
	/* ignore */
}

void server_status(const char *s)
{
	if (strncmp(s, "stream_status ", 14) == 0)
		stream_status(s + 14);
	else if (strncmp(s, "composite_mode ", 15) == 0)
		composite_mode(s + 15);
	else if (strncmp(s, "video_status ", 13) == 0) {
		char *space = strchr(s + 13, ' ');
		if (space == NULL)
			fprintf(stderr,
				"invalid video status \"%s\"\n", s + 13);
		else {
			*space = '\0';
			video_status(s + 13, space + 1);
		}
	} else if (strncmp(s, "message ", 8) == 0)
		message(s + 8);
	else
		fprintf(stderr, "*** %s\n", s);
}
