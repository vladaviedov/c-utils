#define _POSIX_C_SOURCE 200809L
#include "nanorl.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "terminfo.h"

#define START_ALLOC 1024
#define FACTOR 2

#define safe_assign(var_ptr, val) \
	if (var_ptr != NULL) { \
		*var_ptr = val; \
	}

static const char newline = '\n';
static const char backspace = '\b';

static int recvd_signal;
static void sig_handler(int code);
static void shift_arr(char *array, uint32_t length, uint32_t index);
static void unshift_arr(char *array, uint32_t length, uint32_t index);

char *nanorl(const char *prompt, nrl_error *err) {
	return nanorl_fd(STDIN_FILENO, prompt, err);
}

char *nanorl_fd(int fd, const char *prompt, nrl_error *err) {
	nrl_opts options = {
		.fd = fd,
		.prompt = prompt,
		.echo = NRL_ECHO,
		.echo_repl = '\0',
	};

	return nanorl_adv(&options, err);
}

char *nanorl_adv(const nrl_opts *options, nrl_error *err) {
	if (!nrl_load_terminfo()) {
		return NULL;
	}

	if (options->fd < 0) {
		safe_assign(err, NRL_BAD_FD);
		return NULL;
	}
	
	// Setup terminal options
	struct termios old_attr;
	if (tcgetattr(options->fd, &old_attr) < 0) {
		safe_assign(err, NRL_SYS_ERR);
		return NULL;
	}

	struct termios new_attr = old_attr;
	new_attr.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(options->fd, TCSAFLUSH, &new_attr) < 0) {
		safe_assign(err, NRL_SYS_ERR);
		return NULL;
	}

	// Setup signals
	// TODO: need more signals?
	struct sigaction old_sigint_sa;
	struct sigaction old_sigterm_sa;
	struct sigaction old_sigquit_sa;

	struct sigaction new_sa;
	sigemptyset(&new_sa.sa_mask);
	new_sa.sa_handler = &sig_handler;

	if (sigaction(SIGINT, &new_sa, &old_sigint_sa) < 0
		|| sigaction(SIGTERM, &new_sa, &old_sigterm_sa) < 0
		|| sigaction(SIGQUIT, &new_sa, &old_sigquit_sa) < 0) {
		safe_assign(err, NRL_SYS_ERR);
		return NULL;
	}

	// Print prompt:
	write(options->fd, options->prompt, strlen(options->prompt) + 1);

	// Input processing
	char *line_buf = malloc(sizeof(char) * 1024);
	uint32_t alloc_length = 1024;
	uint32_t input_length = 0;
	uint32_t line_cursor = 0;

	ssize_t res;
	char input_buf[4];
	while ((res = read(options->fd, input_buf, sizeof(char) * 4)) > 0 && input_buf[0] != '\n') {
		// Special inputs
		if (res > 1) {
			if (strncmp(input_buf, nrl_lookup_seq(TI_KEY_LEFT), res) == 0) {
				if (line_cursor > 0) {
					line_cursor--;
					write(options->fd, input_buf, res);
				}
			}
			if (strncmp(input_buf, nrl_lookup_seq(TI_KEY_RIGHT), res) == 0) {
				if (line_cursor < input_length) {
					line_cursor++;
					write(options->fd, input_buf, res);
				}
			}

			continue;
		}

		int is_backspace = (input_buf[0] == nrl_lookup_seq(TI_KEY_BACKSPACE)[0]);
		if (is_backspace) {
			// Backspace input
			if (line_cursor == 0) {
				continue;
			}

			unshift_arr(line_buf, input_length, line_cursor - 1);
			line_buf[input_length - 1] = ' ';
			write(options->fd, &backspace, sizeof(char));
		} else {
			// Standard inputs
			if (input_length == alloc_length - 1) {
				line_buf = realloc(line_buf, alloc_length * FACTOR);
			}

			if (line_cursor != input_length) {
				shift_arr(line_buf, input_length, line_cursor);
			}
			line_buf[line_cursor] = input_buf[0];
			input_length++;
			line_cursor++;
		}

		// Redraw
		char *redraw_start = line_buf + (line_cursor - 1);
		uint32_t redraw_length = input_length - (line_cursor - 1);
		switch (options->echo) {
		case NRL_NO_ECHO:
			break;
		case NRL_ECHO:
			write(options->fd, redraw_start, redraw_length * sizeof(char));
			for (uint32_t i = 1; i < redraw_length; i++) {
				write(options->fd, &backspace, sizeof(char));
			}
			break;
		case NRL_FAKE_ECHO:
			for (uint32_t i = 0; i < redraw_length; i++) {
				write(options->fd, &options->echo_repl, sizeof(char));
			}
			for (uint32_t i = 1; i < redraw_length; i++) {
				write(options->fd, &backspace, sizeof(char));
			}
			break;
		}

		// Not work
		if (is_backspace) {
			input_length--;
			line_cursor--;
			write(options->fd, &backspace, sizeof(char));
		}
	}

	// Reset terminal settings
	if (sigaction(SIGINT, &old_sigint_sa, NULL) < 0
		|| sigaction(SIGTERM, &old_sigterm_sa, NULL) < 0
		|| sigaction(SIGQUIT, &old_sigquit_sa, NULL) < 0) {
		safe_assign(err, NRL_SYS_ERR);
		free(line_buf);
		return NULL;
	}

	if (tcsetattr(options->fd, TCSAFLUSH, &old_attr) < 0) {
		safe_assign(err, NRL_SYS_ERR);
		free(line_buf);
		return NULL;
	}

	// Write newline
	write(options->fd, &newline, sizeof(char));
	line_buf[input_length] = '\0';

	// Resend signal to user
	if (res < 0 && errno == EINTR) {
		raise(recvd_signal);
	}

	// Exit
	if (input_length == 0) {
		safe_assign(err, NRL_EMPTY);
		free(line_buf);
		return NULL;
	}

	safe_assign(err, NRL_OK);
	return line_buf;
}

static void sig_handler(int code) {
	recvd_signal = code;
}

static void shift_arr(char *array, uint32_t length, uint32_t index) {
	for (uint32_t i = length; i > index; i--) {
		array[i] = array[i - 1];
	}
}

static void unshift_arr(char *array, uint32_t length, uint32_t index) {
	for (uint32_t i = index; i < length; i++) {
		array[i] = array[i + 1];
	}
}
