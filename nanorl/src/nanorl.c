/**
 * @file nanorl.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version pre1.0
 * @date 2024
 * @license LGPLv3.0
 * @brief Small and simple line editing library.
 */
#define _POSIX_C_SOURCE 200809L
#include "nanorl.h"

#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "printer.h"
#include "terminfo.h"

// Line buffer options
#define START_ALLOC 256
#define FACTOR 2

// Input (1 keystroke) buffer options
#define INPUT_BUF_SIZE 16

#define safe_assign(var_ptr, val)                                              \
	if (var_ptr != NULL) {                                                     \
		*var_ptr = val;                                                        \
	}

static int recvd_signal;
static void sig_handler(int code);

static void shift_str(char *array, uint32_t length, uint32_t index);
static void unshift_str(char *array, uint32_t length, uint32_t index);

static char *noninteractive(int fd, nrl_error *err);

static const char whitespace = ' ';

char *nanorl(const char *prompt, nrl_error *err) {
	return nanorl_fd(STDIN_FILENO, prompt, err);
}

char *nanorl_fd(int fd, const char *prompt, nrl_error *err) {
	nrl_opts options = {
		.fd = fd,
		.prompt = prompt,
		.echo = NRL_ECHO_YES,
		.echo_repl = '\0',
	};

	return nanorl_opts(&options, err);
}

char *nanorl_opts(const nrl_opts *options, nrl_error *err) {
	// Run alternative handling on non-terminals
	if (!isatty(options->fd)) {
		return noninteractive(options->fd, err);
	}

	if (!nrl_load_terminfo()) {
		return NULL;
	}

	if (options->fd < 0) {
		safe_assign(err, NRL_ERR_BAD_FD);
		return NULL;
	}
	nrl_set_fd(options->fd);

	// Setup terminal options
	struct termios old_attr;
	if (tcgetattr(options->fd, &old_attr) < 0) {
		safe_assign(err, NRL_ERR_SYS);
		return NULL;
	}

	struct termios new_attr = old_attr;
	new_attr.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(options->fd, TCSAFLUSH, &new_attr) < 0) {
		safe_assign(err, NRL_ERR_SYS);
		return NULL;
	}

	// Setup signals
	// TODO: need more signals?
	struct sigaction old_sighup_sa;
	struct sigaction old_sigint_sa;
	struct sigaction old_sigterm_sa;
	struct sigaction old_sigquit_sa;

	struct sigaction new_sa;
	sigemptyset(&new_sa.sa_mask);
	new_sa.sa_flags = 0;
	new_sa.sa_handler = &sig_handler;

	if (sigaction(SIGHUP, &new_sa, &old_sighup_sa) < 0
		|| sigaction(SIGINT, &new_sa, &old_sigint_sa) < 0
		|| sigaction(SIGTERM, &new_sa, &old_sigterm_sa) < 0
		|| sigaction(SIGQUIT, &new_sa, &old_sigquit_sa) < 0) {
		safe_assign(err, NRL_ERR_SYS);
		return NULL;
	}

	// Put terminal into application mode
	// See: https://invisible-island.net/xterm/xterm.faq.html#xterm_arrows
	nrl_write_esc(TI_KEYPAD_XMIT);

	// Print prompt:
	nrl_write(options->prompt, strlen(options->prompt));
	nrl_flush();

	// Input processing
	char *line_buf = malloc(START_ALLOC * sizeof(char));
	uint32_t alloc_length = START_ALLOC;
	uint32_t input_length = 0;
	uint32_t line_cursor = 0;

	ssize_t res;
	char input_buf[INPUT_BUF_SIZE];
	while ((res = read(options->fd, input_buf, sizeof(char) * INPUT_BUF_SIZE))
		   > 0) {
		if (input_buf[0] == '\n') {
			break;
		}

		int backspace
			= strncmp(input_buf, nrl_lookup_seq(TI_KEY_BACKSPACE), res) == 0;

		// Special inputs
		if (!backspace && res > 1) {
			if (strncmp(input_buf, nrl_lookup_seq(TI_KEY_LEFT), res) == 0) {
				if (line_cursor > 0) {
					line_cursor--;
					if (options->echo != NRL_ECHO_NO) {
						nrl_write_esc(TI_CURSOR_LEFT);
					}
				}
			} else if (strncmp(input_buf, nrl_lookup_seq(TI_KEY_RIGHT), res)
					   == 0) {
				if (line_cursor < input_length) {
					line_cursor++;
					if (options->echo != NRL_ECHO_NO) {
						nrl_write_esc(TI_CURSOR_RIGHT);
					}
				}
			} else if (strncmp(input_buf, nrl_lookup_seq(TI_KEY_HOME), res)
					   == 0) {
				for (uint32_t i = line_cursor; i > 0; i--) {
					if (options->echo != NRL_ECHO_NO) {
						nrl_write_esc(TI_CURSOR_LEFT);
					}
				}
				line_cursor = 0;
			} else if (strncmp(input_buf, nrl_lookup_seq(TI_KEY_END), res)
					   == 0) {
				for (uint32_t i = line_cursor; i < input_length; i++) {
					if (options->echo != NRL_ECHO_NO) {
						nrl_write_esc(TI_CURSOR_RIGHT);
					}
				}
				line_cursor = input_length;
			}

			nrl_flush();
			continue;
		}

		if (backspace) {
			// Backspace input
			if (line_cursor == 0) {
				continue;
			}

			unshift_str(line_buf, input_length, line_cursor - 1);
			if (options->echo != NRL_ECHO_NO) {
				nrl_write_esc(TI_CURSOR_LEFT);
			}
			line_buf[input_length - 1] = ' ';
		} else {
			// Standard inputs
			if (input_length == alloc_length - 1) {
				alloc_length *= FACTOR;
				line_buf = realloc(line_buf, alloc_length);
			}

			if (line_cursor != input_length) {
				shift_str(line_buf, input_length, line_cursor);
			}
			line_buf[line_cursor] = input_buf[0];
			input_length++;
			line_cursor++;
		}

		// Redraw
		char *redraw_start = line_buf + (line_cursor - 1);
		uint32_t redraw_length = input_length - (line_cursor - 1);
		switch (options->echo) {
		case NRL_ECHO_NO:
			break;
		case NRL_ECHO_YES:
			nrl_write(redraw_start, redraw_length * sizeof(char));
			break;
		case NRL_ECHO_FAKE:
			for (uint32_t i = 0; i < redraw_length - 1; i++) {
				nrl_write(&options->echo_repl, sizeof(char));
			}

			const char *final = backspace ? &whitespace : &options->echo_repl;
			nrl_write(final, sizeof(char));
			break;
		}

		if (backspace) {
			input_length--;
			line_cursor--;
		}

		if (options->echo != NRL_ECHO_NO) {
			for (uint32_t i = 1; i < redraw_length + backspace; i++) {
				nrl_write_esc(TI_CURSOR_LEFT);
			}

			nrl_flush();
		}
	}

	// Reset terminal settings
	if (sigaction(SIGHUP, &old_sighup_sa, NULL) < 0
		|| sigaction(SIGINT, &old_sigint_sa, NULL) < 0
		|| sigaction(SIGTERM, &old_sigterm_sa, NULL) < 0
		|| sigaction(SIGQUIT, &old_sigquit_sa, NULL) < 0) {
		safe_assign(err, NRL_ERR_SYS);
		free(line_buf);
		return NULL;
	}

	if (tcsetattr(options->fd, TCSAFLUSH, &old_attr) < 0) {
		safe_assign(err, NRL_ERR_SYS);
		free(line_buf);
		return NULL;
	}

	// Put terminal into local mode
	nrl_write_esc(TI_KEYPAD_LOCAL);

	// Write newline
	char nl_buf = '\n';
	nrl_write(&nl_buf, sizeof(char));
	nrl_flush();

	// Resend signal to user
	if (res < 0 && errno == EINTR) {
		raise(recvd_signal);
	}

	// Exit
	if (input_length == 0) {
		safe_assign(err, NRL_ERR_EMPTY);
		free(line_buf);
		return NULL;
	}

	// Null-terminate
	line_buf[input_length] = '\0';

	safe_assign(err, NRL_ERR_OK);
	return line_buf;
}

static void sig_handler(int code) {
	recvd_signal = code;
}

/**
 * @brief Insert element into string, shifting elements forward.
 *
 * @param[in,out] array - String.
 * @param[in] length - Current length of string.
 * @param[in] index - Index to insert into.
 */
static void shift_str(char *array, uint32_t length, uint32_t index) {
	for (uint32_t i = length; i > index; i--) {
		array[i] = array[i - 1];
	}
}

/**
 * @brief Erase element in string, shifting elements back.
 *
 * @param[in,out] array - String.
 * @param[in] length - Current length of string.
 * @param[in] index - Index to erase.
 */
static void unshift_str(char *array, uint32_t length, uint32_t index) {
	for (uint32_t i = index; i < length; i++) {
		array[i] = array[i + 1];
	}
}

/**
 * @brief Non-interactive input handling.
 *
 * @param[in] fd - Input file descriptor.
 * @param[out] err - Error code buffer.
 * @return Input string.
 */
static char *noninteractive(int fd, nrl_error *err) {
	char *line_buf = malloc(START_ALLOC * sizeof(char));
	uint32_t alloc_length = START_ALLOC;
	uint32_t input_length = 0;

	ssize_t res;
	char input_buf[START_ALLOC];
	while ((res = read(fd, input_buf, sizeof(char) * START_ALLOC)) > 0) {
		if (input_length + res > alloc_length - 1) {
			alloc_length *= FACTOR;
			line_buf = realloc(line_buf, alloc_length);
		}

		memcpy(line_buf + input_length, input_buf, res);
		input_length += res;
	}

	if (input_length == 0) {
		free(line_buf);
		safe_assign(err, NRL_ERR_EMPTY);
		return NULL;
	}

	// Null-terminate
	line_buf[input_length - 1] = '\0';

	safe_assign(err, NRL_ERR_OK);
	return line_buf;
}
