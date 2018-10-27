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

static enum source {
	s_off,
	s_projection,
	s_cam1,
	s_cam2,
	s_extra,
	s_unknown = -1
} srca_active, srca_toggle, srcb_active, srcb_toggle;

static enum mode {
	m_sbs_equal,
	m_sbs_preview,
	m_picture_in_picture,
	m_unknown = -1
} mode_active, mode_toggle, mode_compose;

static enum led LED_MATRIX_ACTIVE[] = {
	led_k1red,
	led_k2red,
	led_k3red,
	led_k4red,
	led_k5red,
	led_k6red,
	led_k7red,
	led_k8red,
	led_k9red,
	led_k10red,
	led_k11red,
	led_k12red,
	led_k13red,
	led_k14red,
	led_k15red,
	led_k16red,
	led_k17red
};
static enum led LED_MATRIX_TOGGLE[] = {
	led_k1green,
	led_k2green,
	led_k3green,
	led_k4green,
	led_k5green,
	led_k6green,
	led_k7green,
	led_k8green,
	led_k9green,
	led_k10green,
	led_k11green,
	led_k12green,
	led_k13green,
	led_k14green,
	led_k15green,
	led_k16green,
	led_k17green
};

static enum led LED_MODE_ACTIVE[] = {
	led_k18red,
	led_k19red,
	led_k20red
};
static enum led LED_MODE_TOGGLE[] = {
	led_k18green,
	led_k19green,
	led_k20green
};

static const char *source_names_for_server[] = {
	"*",
	"slides",
	"cam1",
	"cam2",
	"cam3",
	"<undefined>"
};
static const char *mode_names_for_server[] = {
	"side_by_side_equal",
	"side_by_side_preview",
	"picture_in_picture",
	"<undefined>"
};


void init()
{
	srca_active = s_unknown;
	srca_toggle = s_unknown;
	srcb_active = s_unknown;
	srcb_toggle = s_unknown;

	mode_active = m_unknown;
	mode_toggle = m_unknown;
	mode_compose = m_unknown;
}

int blink(void)
{
	if (mode_compose == m_unknown)
		return -1;

	static bool state = 0;
	set_led(LED_MODE_ACTIVE[mode_compose], state);
	state = !state;
	return 100;
}


static unsigned int matrix(enum source srca, enum source srcb)
{
	switch (srca) {
	case s_off:
		return 17;
	case s_projection:
		switch (srcb) {
		case s_off:	    return 4;
		case s_cam1:	    return 3;
		case s_cam2:	    return 2;
		case s_extra:	    return 1;
		}
		break;
	case s_cam1:
		switch (srcb) {
		case s_off:	    return 8;
		case s_projection:  return 7;
		case s_cam2:	    return 6;
		case s_extra:	    return 5;
		}
		break;
	case s_cam2:
		switch (srcb) {
		case s_off:	    return 12;
		case s_projection:  return 11;
		case s_cam1:	    return 10;
		case s_extra:	    return 9;
		}
		break;
	case s_extra:
		switch (srcb) {
		case s_off:	    return 16;
		case s_projection:  return 15;
		case s_cam1:	    return 14;
		case s_cam2:	    return 13;
		}
		break;
	}
	return 0;
}

static void update_leds()
{
	unsigned int i, lit;

	for (i = 0; i < 3; i++)
		set_led(LED_MODE_ACTIVE[i], mode_active == i ||
					    mode_compose == i);
	for (i = 0; i < 3; i++)
		set_led(LED_MODE_TOGGLE[i], mode_toggle == i);

	lit = matrix(srca_active, srcb_active) - 1;
	for (i = 0; i < 17; i++)
		set_led(LED_MATRIX_ACTIVE[i], i == lit);

	lit = matrix(srca_toggle, srcb_toggle) - 1;
	for (i = 0; i < 17; i++)
		set_led(LED_MATRIX_TOGGLE[i], i == lit);
}


static void set_state(enum source srca_new,
		      enum source srcb_new, bool toggle)
{
	enum mode mode_new = toggle ? mode_toggle :
		mode_compose != m_unknown ? mode_compose : mode_active;
	mode_compose = m_unknown;

	if (srca_new == s_off) {
		send_cmd("set_stream_blank pause\n");
	} else if (srcb_new == s_off) {
		char cmd[1024];
		snprintf(cmd, sizeof cmd,
			 "set_videos_and_composite %s * fullscreen\n",
			 source_names_for_server[srca_new]);
		send_cmd(cmd);
		send_cmd("set_stream_live\n");
	} else {
		char cmd[1024];
		snprintf(cmd, sizeof cmd,
			 "set_videos_and_composite %s %s %s\n",
			 source_names_for_server[srca_new],
			 source_names_for_server[srcb_new],
			 mode_names_for_server[mode_new]);
		send_cmd(cmd);
		send_cmd("set_stream_live\n");
	}

	/* don't update XXX_active; wait for server response instead */

	if (mode_new != mode_active || srca_new != srca_active ||
				       srcb_new != srcb_active) {
		mode_toggle = mode_active;
		srca_toggle = srca_active;
		srcb_toggle = srcb_active;
	}

	if (srca_toggle == s_off) {
		send_cmd("message green * * pause\n");
	} else if (srcb_toggle == s_off) {
		char cmd[1024];
		snprintf(cmd, sizeof cmd,
			 "message green %s * fullscreen\n",
			 source_names_for_server[srca_toggle]);
		send_cmd(cmd);
	} else {
		char cmd[1024];
		snprintf(cmd, sizeof cmd,
			 "message green %s %s %s\n",
			 source_names_for_server[srca_toggle],
			 source_names_for_server[srcb_toggle],
			 mode_names_for_server[mode_toggle]);
		send_cmd(cmd);
	}

	update_leds();
}

void button(int i)
{
	switch (i + 1) {
	case  1: set_state(s_projection, s_extra,      false); break;
	case  2: set_state(s_projection, s_cam2,       false); break;
	case  3: set_state(s_projection, s_cam1,       false); break;
	case  4: set_state(s_projection, s_off,        false); break;
	case  5: set_state(s_cam1,       s_extra,      false); break;
	case  6: set_state(s_cam1,       s_cam2,       false); break;
	case  7: set_state(s_cam1,       s_projection, false); break;
	case  8: set_state(s_cam1,       s_off,        false); break;
	case  9: set_state(s_cam2,       s_extra,      false); break;
	case 10: set_state(s_cam2,       s_cam1,       false); break;
	case 11: set_state(s_cam2,       s_projection, false); break;
	case 12: set_state(s_cam2,       s_off,        false); break;
	case 13: set_state(s_extra,      s_cam2,       false); break;
	case 14: set_state(s_extra,      s_cam1,       false); break;
	case 15: set_state(s_extra,      s_projection, false); break;
	case 16: set_state(s_extra,      s_off,        false); break;
	case 17: set_state(s_off,        s_off,        false); break;
	case 18:
	case 19:
	case 20:
		if (mode_active == i - 17 || mode_compose == i - 17)
			mode_compose = m_unknown;
		else
			mode_compose = i - 17;
		update_leds();
		break;
	case 21:
		set_state(srca_toggle, srcb_toggle, true);
		break;
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
		switch (server_video_status_a) {
		case vs_cam1: srca_active = s_cam1; break;
		case vs_cam2: srca_active = s_cam2; break;
		case vs_cam3: srca_active = s_extra; break;
		case vs_slides: srca_active = s_projection; break;
		}
		switch (server_video_status_b) {
		case vs_cam1: srcb_active = s_cam1; break;
		case vs_cam2: srcb_active = s_cam2; break;
		case vs_cam3: srcb_active = s_extra; break;
		case vs_slides: srcb_active = s_projection; break;
		}

		if (server_stream_status != ss_live) {
			srca_active = s_off;
			srcb_active = s_off;
		} else switch (server_composite_mode) {
		case cm_fullscreen:
			srcb_active = s_off;
			break;
		case cm_side_by_side_equal:
			mode_active = m_sbs_equal;
			break;
		case cm_side_by_side_preview:
			mode_active = m_sbs_preview;
			break;
		case cm_picture_in_picture:
			mode_active = m_picture_in_picture;
			break;
		}

		if (mode_active == m_unknown)
			mode_active = mode_toggle != m_unknown
				? mode_toggle : m_picture_in_picture;

		if (mode_toggle == m_unknown || srca_toggle == s_unknown ||
						srcb_toggle == s_unknown) {
			mode_toggle = mode_active;
			srca_toggle = srca_active;
			srcb_toggle = srcb_active;
		}
	}

	update_leds();
}
