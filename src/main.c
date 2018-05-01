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

#include "voctoknopf.h"

#define EXTRA_LED "/sys/class/leds/extra_led"
#define BUTTON_N "/sys/class/gpio/k%d"
#define IRQ_N "/sys/class/gpio/u%dirq"

static const char *led_gpio[LED_COUNT] = {
	"/sys/class/gpio/k1green",
	"/sys/class/gpio/k1red",
	"/sys/class/gpio/k2green",
	"/sys/class/gpio/k2red",
	"/sys/class/gpio/k3green",
	"/sys/class/gpio/k3red",
	"/sys/class/gpio/k4green",
	"/sys/class/gpio/k4red",
	"/sys/class/gpio/k5green",
	"/sys/class/gpio/k5red",
	"/sys/class/gpio/k6blue",
	"/sys/class/gpio/k7blue",
	"/sys/class/gpio/k8blue",
	"/sys/class/gpio/k9blue",
	"/sys/class/gpio/k10blue",
	"/sys/class/gpio/k11green",
	"/sys/class/gpio/k11red",
	"/sys/class/gpio/k12green",
	"/sys/class/gpio/k12red",
	"/sys/class/gpio/k13green",
	"/sys/class/gpio/k13red",
	"/sys/class/gpio/k14green",
	"/sys/class/gpio/k14red",
	"/sys/class/gpio/k15green",
	"/sys/class/gpio/k15red",
	"/sys/class/gpio/k16green",
	"/sys/class/gpio/k16red",
	"/sys/class/gpio/k17green",
	"/sys/class/gpio/k17red",
	"/sys/class/gpio/k18green",
	"/sys/class/gpio/k18red",
	"/sys/class/gpio/k19green",
	"/sys/class/gpio/k19red",
	"/sys/class/gpio/k20green",
	"/sys/class/gpio/k20red"
};
static FILE *led_stream[LED_COUNT];
static FILE *extra_led_stream;

static int button_fd[21];
static bool button_state[21];

static int irq_fd[4];

static int sockfd;

static char sockbuf[1024];
static size_t sockbuf_used = 0;
static bool sockbuf_overflow = false;

static bool quit = false;


void set_led(enum led led, bool value)
{
	if (fprintf(led_stream[led], "%d\n", value) < 0) {
		fprintf(stderr, "%s: fprintf: %s (fd %d): %m\n",
			__func__, led_gpio[led], fileno(led_stream[led]));
		exit(EXIT_FAILURE);
	}
}

static void set_extra_led(bool on)
{
	if (fprintf(extra_led_stream, "%d\n", on ? 255 : 0) < 0) {
		fprintf(stderr, "%s: fprintf: fd %d: %m\n",
			__func__, fileno(extra_led_stream));
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
	bool newstate[21];
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

		for (unsigned int i = bank * 5;
		     i < (bank + 1) * 5 + (bank == 3); i++)
			newstate[i] = get_button_state(i);

		if (poll(&pollfd, 1, 0) == -1) {
			fprintf(stderr, "%s: poll: %m\n", __func__);
			exit(EXIT_FAILURE);
		}
	} while (pollfd.revents & POLLPRI);

	for (unsigned int i = bank * 5;
	     i < (bank + 1) * 5 + (bank == 3); i++) {
		if (!button_state[i] && newstate[i])
			switch (i) {
			case 0: case 1: case 2: case 3: case 4:
				bank0(i); break;
			case 5: case 6: case 7: case 8: case 9:
				bank1(i - 5); break;
			case 10: case 11: case 12: case 13: case 14:
				bank2(i - 10); break;
			case 15: case 16: case 17: case 18: case 19:
				bank3(i - 15); break;
			case 20:
				take(); break;
			}
		button_state[i] = newstate[i];
	}
}


void send_cmd(const char *s)
{
	size_t count = strlen(s);

	while (count) {
		ssize_t w = write(sockfd, s, count);

		if (w == -1) {
			fprintf(stderr, "%s: write: fd %d: %m\n",
					__func__, sockfd);
			exit(EXIT_FAILURE);
		}
		if (w == 0) {
			fprintf(stderr, "%s: write: fd %d: End of file\n",
					__func__, sockfd);
			exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
	case 0:
		fprintf(stderr, "%s: read: fd %d: End of file\n",
				__func__, sockfd);
		exit(EXIT_FAILURE);
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
	(void) fprintf(extra_led_stream, "0\n");

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

	extra_led_stream = fopen(EXTRA_LED "/brightness", "w");
	if (extra_led_stream == NULL) {
		fprintf(stderr, "fopen: %s: %m\n", EXTRA_LED "/brightness");
		exit(EXIT_FAILURE);
	}
	if (setvbuf(extra_led_stream, NULL, _IONBF, 0) != 0) {
		fprintf(stderr, "setvbuf: %s (fd %d): %m\n",
			EXTRA_LED "/brightness", fileno(extra_led_stream));
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < 21; i++) {
		char pathname[BUFSIZ];
		if (snprintf(pathname, sizeof pathname,
			     BUTTON_N "/value", i + 1)
			    >= sizeof pathname) {
			fprintf(stderr, "GPIO pathname too long\n");
			exit(EXIT_FAILURE);
		}
		button_fd[i] = open(pathname, O_RDONLY);
		if (button_fd[i] == -1) {
			fprintf(stderr, "open: %s: %m\n", pathname);
			exit(EXIT_FAILURE);
		}
	}

	for (unsigned int i = 0; i < 4; i++) {
		char pathname[BUFSIZ];

		if (snprintf(pathname, sizeof pathname,
			     IRQ_N "/edge", i + 1)
		    >= sizeof pathname) {
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
			     IRQ_N "/value", i + 1)
		    >= sizeof pathname) {
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

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		fprintf(stderr, "socket: %m\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in sa = {
		.sin_family = AF_INET,
		.sin_port = htons(9999)
	};
	if (inet_aton(argv[1], &sa.sin_addr) == 0) {
		fprintf(stderr, "Invalid host address \"%s\"\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	if (connect(sockfd, (struct sockaddr *)&sa, sizeof sa) == -1) {
		fprintf(stderr, "connect: host %s, port %hd: %m\n",
				inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
		exit(EXIT_FAILURE);
	}

	send_cmd("get_stream_status\n"
		 "get_composite_mode\n"
		 "get_video\n");

	if (atexit(quench_leds) == -1) {
		fprintf(stderr, "atexit failed\n");
		exit(EXIT_FAILURE);
	}
	set_extra_led(true);

	struct pollfd fds[5] = {
		{ .fd = irq_fd[0], .events = POLLPRI },
		{ .fd = irq_fd[1], .events = POLLPRI },
		{ .fd = irq_fd[2], .events = POLLPRI },
		{ .fd = irq_fd[3], .events = POLLPRI },
		{ .fd = sockfd,    .events = POLLIN }
	};
	do {
		if (poll(fds, 5, -1) == -1 && errno != EINTR) {
			fprintf(stderr, "poll: %m\n");
			exit(EXIT_FAILURE);
		}

		if (fds[0].revents & POLLPRI)
			read_bank(0);
		if (fds[1].revents & POLLPRI)
			read_bank(1);
		if (fds[2].revents & POLLPRI)
			read_bank(2);
		if (fds[3].revents & POLLPRI)
			read_bank(3);
		if (fds[4].revents & POLLIN)
			sock_read();
	} while (!quit);

	fprintf(stderr, "... quit\n");
	return EXIT_SUCCESS;
}
