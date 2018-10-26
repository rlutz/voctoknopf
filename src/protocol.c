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

enum server_stream_status server_stream_status;
enum server_composite_mode server_composite_mode,
			   green_composite_mode;
enum server_video_status server_video_status_a, server_video_status_b,
			 green_video_status_a, green_video_status_b;


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

	server_status_changed();
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

	server_status_changed();
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

	server_status_changed();
}

static void message(char *arg)
{
	char *space, *arg_a, *arg_b;

	if (strncmp(arg, "green ", 6) != 0)
		return;

	arg_a = arg + 6;
	space = strchr(arg_a, ' ');
	if (space == NULL) {
		fprintf(stderr, "invalid message \"%s\"\n", arg);
		return;
	}
	arg_b = space + 1;
	space = strchr(arg_b, ' ');
	if (space == NULL) {
		fprintf(stderr, "invalid message \"%s\"\n", arg);
		return;
	}
	arg = space + 1;

	arg_b[-1] = '\0';
	arg[-1] = '\0';

	if (strcmp(arg, "fullscreen") == 0)
		green_composite_mode = cm_fullscreen;
	else if (strcmp(arg, "side_by_side_equal") == 0)
		green_composite_mode = cm_side_by_side_equal;
	else if (strcmp(arg, "side_by_side_preview") == 0)
		green_composite_mode = cm_side_by_side_preview;
	else if (strcmp(arg, "picture_in_picture") == 0)
		green_composite_mode = cm_picture_in_picture;
	else
		green_composite_mode = cm_unknown;

	if (strcmp(arg_a, "cam1") == 0)
		green_video_status_a = vs_cam1;
	else if (strcmp(arg_a, "cam2") == 0)
		green_video_status_a = vs_cam2;
	else if (strcmp(arg_a, "cam3") == 0)
		green_video_status_a = vs_cam3;
	else if (strcmp(arg_a, "slides") == 0)
		green_video_status_a = vs_slides;
	else
		green_video_status_a = vs_unknown;

	if (strcmp(arg_b, "cam1") == 0)
		green_video_status_b = vs_cam1;
	else if (strcmp(arg_b, "cam2") == 0)
		green_video_status_b = vs_cam2;
	else if (strcmp(arg_b, "cam3") == 0)
		green_video_status_b = vs_cam3;
	else if (strcmp(arg_b, "slides") == 0)
		green_video_status_b = vs_slides;
	else
		green_video_status_b = vs_unknown;

	green_status_changed();
}

void server_status(char *s)
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
