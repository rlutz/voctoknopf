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
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pwd.h>
#include <netinet/tcp.h>

#include "voctoknopf.h"

static FILE *led_stream[LED_COUNT];
static FILE *status_led_stream;

static int button_fd[BUTTON_COUNT];
static bool button_state[BUTTON_COUNT];

static int irq_fd[IRQ_COUNT];

static int sockfd;

static char sockbuf[1024];
static size_t sockbuf_used;
static bool sockbuf_overflow;

static bool connected = false;

static bool quit = false;


void set_led(enum led led, bool value)
{
	if (fprintf(led_stream[led], "%d\n", value) < 0) {
		fprintf(stderr, "%s: fprintf: %s (fd %d): %m\n",
			__func__, led_gpio[led], fileno(led_stream[led]));
		exit(EXIT_FAILURE);
	}
}

static void set_status_led(bool on)
{
	if (status_led_stream == NULL)
		return;

	if (fprintf(status_led_stream, "%d\n", on ? 255 : 0) < 0) {
		fprintf(stderr, "%s: fprintf: fd %d: %m\n",
			__func__, fileno(status_led_stream));
		exit(EXIT_FAILURE);
	}
}


static bool get_button_state(int i)
{
	char gpiobuf[4];
	ssize_t count;

	if (lseek(button_fd[i], 0, SEEK_SET) == -1) {
		fprintf(stderr, "%s: lseek: fd %d: %m\n",
				__func__, button_fd[i]);
		exit(EXIT_FAILURE);
	}
	count = read(button_fd[i], gpiobuf, sizeof gpiobuf);
	if (count == -1) {
		fprintf(stderr, "%s: read: fd %d: %m\n",
				__func__, button_fd[i]);
		exit(EXIT_FAILURE);
	}

	if (count == 2 && gpiobuf[1] == '\n')
		switch (gpiobuf[0]) {
		case '0': return false;
		case '1': return true;
		}

	fprintf(stderr, "%s: fd %d: read garbage\n",
			__func__, button_fd[i]);
	exit(EXIT_FAILURE);
}

static void read_bank(unsigned int bank)
{
	bool newstate[BUTTON_COUNT];
	struct pollfd pollfd;
	pollfd.fd = irq_fd[bank];
	pollfd.events = POLLPRI;

	do {
		char gpiobuf[4];
		if (lseek(irq_fd[bank], 0, SEEK_SET) == -1) {
			fprintf(stderr, "%s: lseek: fd %d: %m\n",
					__func__, irq_fd[bank]);
			exit(EXIT_FAILURE);
		}
		if (read(irq_fd[bank], gpiobuf, sizeof gpiobuf) == -1) {
			fprintf(stderr, "%s: read: fd %d: %m\n",
					__func__, irq_fd[bank]);
			exit(EXIT_FAILURE);
		}

		for (unsigned int i = 0; i < BUTTON_COUNT; i++)
			newstate[i] = get_button_state(i);

		if (poll(&pollfd, 1, 0) == -1) {
			fprintf(stderr, "%s: poll: %m\n", __func__);
			exit(EXIT_FAILURE);
		}
	} while (pollfd.revents & POLLPRI);

	if (!connected)
		return;

	for (unsigned int i = 0; i < BUTTON_COUNT; i++) {
		if (!button_state[i] && newstate[i])
			button(i);
		button_state[i] = newstate[i];
	}
}


static void try_connect(const char *host)
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		fprintf(stderr, "socket: %m\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in sa = {
		.sin_family = AF_INET,
		.sin_port = htons(9999)
	};
	if (inet_aton(host, &sa.sin_addr) == 0) {
		fprintf(stderr, "Invalid host address \"%s\"\n", host);
		exit(EXIT_FAILURE);
	}
	if (connect(sockfd, (struct sockaddr *)&sa, sizeof sa) == -1) {
		fprintf(stderr, "connect: host %s, port %hd: %m\n",
				inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
		if (close(sockfd) == -1)
			fprintf(stderr, "%s: close: fd %d: %m\n",
					__func__, sockfd);
		sockfd = -1;
		return;
	}

	int keepalive = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
		       &keepalive, sizeof keepalive) == -1) {
		fprintf(stderr, "%s: setsockopt: SO_KEEPALIVE: %m\n",
				__func__);
		exit(EXIT_FAILURE);
	}

	int keepidle = 1, keepintvl = 1, keepcnt = 1;
	if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE,
		       &keepidle, sizeof keepidle) == -1) {
		fprintf(stderr, "%s: setsockopt: TCP_KEEPIDLE: %m\n",
				__func__);
		exit(EXIT_FAILURE);
	}
	if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL,
		       &keepintvl, sizeof keepintvl) == -1) {
		fprintf(stderr, "%s: setsockopt: TCP_KEEPINTVL: %m\n",
				__func__);
		exit(EXIT_FAILURE);
	}
	if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT,
		       &keepcnt, sizeof keepcnt) == -1) {
		fprintf(stderr, "%s: setsockopt: TCP_KEEPCNT: %m\n",
				__func__);
		exit(EXIT_FAILURE);
	}

	unsigned int user_timeout = 1000;
	if (setsockopt(sockfd, IPPROTO_TCP, TCP_USER_TIMEOUT,
		       &user_timeout, sizeof user_timeout) == -1) {
		fprintf(stderr, "%s: setsockopt TCP_USER_TIMEOUT: %m\n",
				__func__);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "connected to %s:%d\n",
		inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));

	connected = true;
	sockbuf_used = 0;
	sockbuf_overflow = false;

	set_status_led(true);

	server_stream_status = ss_unknown;
	server_composite_mode = cm_unknown;
	server_video_status_a = vs_unknown;
	server_video_status_b = vs_unknown;

	send_cmd("get_stream_status\n"
		 "get_composite_mode\n"
		 "get_video\n");

	init();
}

static void disconnect()
{
	if (close(sockfd) == -1)
		fprintf(stderr, "%s: close: fd %d: %m\n", __func__, sockfd);
	sockfd = -1;
	connected = false;

	set_status_led(false);
	for (unsigned int i = 0; i < LED_COUNT; i++)
		set_led(i, false);
}

void send_cmd(const char *s)
{
	size_t count = strlen(s);

	while (count) {
		ssize_t w = write(sockfd, s, count);

		if (w == -1) {
			fprintf(stderr, "%s: write: fd %d: %m\n",
					__func__, sockfd);
			disconnect();
			return;
		}
		if (w == 0) {
			fprintf(stderr, "%s: write: fd %d: End of file\n",
					__func__, sockfd);
			disconnect();
			return;
		}

		s += w;
		count -= w;
	}
}

static void sock_read()
{
	ssize_t r = read(sockfd, sockbuf + sockbuf_used,
				 sizeof sockbuf - sockbuf_used);
	switch (r) {
	case -1:
		fprintf(stderr, "%s: read: fd %d: %m\n", __func__, sockfd);
		disconnect();
		return;
	case 0:
		fprintf(stderr, "%s: read: fd %d: End of file\n",
				__func__, sockfd);
		disconnect();
		return;
	}
	sockbuf_used += r;

	char *start = sockbuf, *nl;

	while ((nl = memchr(start, '\n', sockbuf_used)) != NULL) {
		*nl = '\0';

		if (sockbuf_overflow)
			sockbuf_overflow = false;
		else
			server_status(start);

		sockbuf_used -= nl + 1 - start;
		start = nl + 1;
	}

	if (start != sockbuf)
		memmove(sockbuf, start, sockbuf_used);

	if (sockbuf_overflow)
		sockbuf_used = 0;
	else if (sockbuf_used == sizeof sockbuf) {
		fprintf(stderr, "Received overlong line, ignoring...\n");
		sockbuf_used = 0;
		sockbuf_overflow = true;
	}
}


static void quench_leds()
{
	if (status_led_stream != NULL)
		(void) fprintf(status_led_stream, "0\n");

	for (unsigned int i = 0; i < LED_COUNT; i++)
		(void) fprintf(led_stream[i], "0\n");
}

static void sighandler(int sig)
{
	quit = true;
}

int main(int argc, char* argv[])
{
	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "Usage: %s HOST\n", argv[0]);
		exit(argc == 2 ? EXIT_SUCCESS : EXIT_FAILURE);
	}
	if (strcmp(argv[1], "--version") == 0) {
		fprintf(stderr, "Voctoknopf - video mixer control device ");
		fprintf(stderr, "for Chaos conferences\n");
		fprintf(stderr, "Copyright (C) 2018 Roland Lutz\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "This program is free software; you can ");
		fprintf(stderr, "redistribute it and/or\nmodify it under the ");
		fprintf(stderr, "terms of the GNU General Public License\nas ");
		fprintf(stderr, "published by the Free Software Foundation; ");
		fprintf(stderr, "either version 2\nof the License, or (at ");
		fprintf(stderr, "your option) any later version.\n");
		fprintf(stderr, "\n");
		fprintf(stderr, "This program is distributed in the hope ");
		fprintf(stderr, "that it will be useful,\nbut WITHOUT ANY ");
		fprintf(stderr, "WARRANTY; without even the implied warranty ");
		fprintf(stderr, "of\nMERCHANTABILITY or FITNESS FOR A ");
		fprintf(stderr, "PARTICULAR PURPOSE.  See the\nGNU General ");
		fprintf(stderr, "Public License for more details.\n");
		exit(EXIT_SUCCESS);
	}

	struct sigaction sact = { .sa_handler = sighandler, .sa_flags = 0 };
	sigemptyset(&sact.sa_mask);
	sigaction(SIGINT, &sact, NULL);
	sigaction(SIGTERM, &sact, NULL);

	for (unsigned int i = 0; i < LED_COUNT; i++) {
		char pathname[BUFSIZ];
		if (snprintf(pathname, sizeof pathname,
			     "%s/value", led_gpio[i]) >= sizeof pathname) {
			fprintf(stderr, "GPIO pathname too long\n");
			exit(EXIT_FAILURE);
		}
		led_stream[i] = fopen(pathname, "w");
		if (led_stream[i] == NULL) {
			fprintf(stderr, "fopen: %s: %m\n", pathname);
			exit(EXIT_FAILURE);
		}
		errno = 0;
		if (setvbuf(led_stream[i], NULL, _IONBF, 0) != 0) {
			fprintf(stderr, "setvbuf: %s (fd %d): %m\n",
					pathname, fileno(led_stream[i]));
			exit(EXIT_FAILURE);
		}
		if (fprintf(led_stream[i], "0\n") < 0) {
			fprintf(stderr, "fprintf: %s (fd %d): %m\n",
					pathname, fileno(led_stream[i]));
			exit(EXIT_FAILURE);
		}
	}

	if (status_led == NULL)
		status_led_stream = NULL;
	else {
		char pathname[BUFSIZ];
		if (snprintf(pathname, sizeof pathname,
			     "%s/brightness", status_led) >= sizeof pathname) {
			fprintf(stderr, "GPIO pathname too long\n");
			exit(EXIT_FAILURE);
		}
		status_led_stream = fopen(pathname, "w");
		if (status_led_stream == NULL) {
			fprintf(stderr, "fopen: %s: %m\n", pathname);
			exit(EXIT_FAILURE);
		}
		errno = 0;
		if (setvbuf(status_led_stream, NULL, _IONBF, 0) != 0) {
			fprintf(stderr, "setvbuf: %s (fd %d): %m\n",
					pathname, fileno(status_led_stream));
			exit(EXIT_FAILURE);
		}
		if (fprintf(status_led_stream, "0\n") < 0) {
			fprintf(stderr, "fprintf: %s (fd %d): %m\n",
					pathname, fileno(status_led_stream));
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < BUTTON_COUNT; i++) {
		char pathname[BUFSIZ];
		if (snprintf(pathname, sizeof pathname,
			     "%s/value", button_gpio[i]) >= sizeof pathname) {
			fprintf(stderr, "GPIO pathname too long\n");
			exit(EXIT_FAILURE);
		}
		button_fd[i] = open(pathname, O_RDONLY);
		if (button_fd[i] == -1) {
			fprintf(stderr, "open: %s: %m\n", pathname);
			exit(EXIT_FAILURE);
		}
	}

	for (unsigned int i = 0; i < IRQ_COUNT; i++) {
		char pathname[BUFSIZ];

		if (snprintf(pathname, sizeof pathname,
			     "%s/edge", irq_gpio[i]) >= sizeof pathname) {
			fprintf(stderr, "GPIO pathname too long\n");
			exit(EXIT_FAILURE);
		}
		FILE *f = fopen(pathname, "w");
		if (f == NULL) {
			fprintf(stderr, "fopen: %s: %m\n", pathname);
			exit(EXIT_FAILURE);
		}
		if (fprintf(f, "falling\n") < 0) {
			fprintf(stderr, "fprintf: %s (fd %d): %m\n",
					pathname, fileno(f));
			exit(EXIT_FAILURE);
		}
		if (fclose(f) == EOF) {
			fprintf(stderr, "fclose: %s (fd %d): %m\n",
					pathname, fileno(f));
			exit(EXIT_FAILURE);
		}

		if (snprintf(pathname, sizeof pathname,
			     "%s/value", irq_gpio[i]) >= sizeof pathname) {
			fprintf(stderr, "GPIO pathname too long\n");
			exit(EXIT_FAILURE);
		}
		irq_fd[i] = open(pathname, O_RDONLY);
		if (irq_fd[i] == -1) {
			fprintf(stderr, "open: %s: %m\n", pathname);
			exit(EXIT_FAILURE);
		}
		read_bank(i);
	}

	if (atexit(quench_leds) == -1) {
		fprintf(stderr, "atexit failed\n");
		exit(EXIT_FAILURE);
	}

	const char *name = "nobody";
	struct passwd *pwd;
	errno = 0;
	pwd = getpwnam(name);
	if (pwd == NULL) {
		if (errno == 0)
			fprintf(stderr, "%s: getpwnam: "
					"User \"%s\" not found\n", name);
		else
			fprintf(stderr, "%s: getpwnam: %m\n", __func__);
		exit(EXIT_FAILURE);
	}

	if (setgroups(0, NULL) == -1) {
		fprintf(stderr, "%s: setgroups: %m\n", __func__);
		exit(EXIT_FAILURE);
	}
	if (setgid(pwd->pw_gid) == -1) {
		fprintf(stderr, "%s: setgid: gid %d: %m\n",
				__func__, pwd->pw_gid);
		exit(EXIT_FAILURE);
	}
	if (setuid(pwd->pw_uid) == -1) {
		fprintf(stderr, "%s: setuid: uid %d: %m\n",
				__func__, pwd->pw_uid);
		exit(EXIT_FAILURE);
	}

	struct pollfd fds[IRQ_COUNT + 1];
	memset(&fds, 0, sizeof fds);
	for (unsigned int i = 0; i < IRQ_COUNT; i++) {
		fds[i].fd = irq_fd[i];
		fds[i].events = POLLPRI;
	}
	fds[IRQ_COUNT].events = POLLIN;

	do {
		if (!connected)
			try_connect(argv[1]);

		fds[IRQ_COUNT].fd = sockfd;
		if ((connected ? poll(fds, IRQ_COUNT + 1, -1)
			       : poll(fds, IRQ_COUNT, 1000)) == -1
		    && errno != EINTR) {
			fprintf(stderr, "poll: %m\n");
			exit(EXIT_FAILURE);
		}

		for (unsigned int i = 0; i < IRQ_COUNT; i++)
			if (fds[i].revents & POLLPRI)
				read_bank(i);
		if (fds[IRQ_COUNT].revents & POLLIN && connected)
			sock_read();
	} while (!quit);

	fprintf(stderr, "... quit\n");
	return EXIT_SUCCESS;
}
