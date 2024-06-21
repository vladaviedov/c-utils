/**
 * @file nanorl.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.1
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

#include "dfa.h"
#include "io.h"
#include "terminfo.h"

// Line buffer options
#define START_ALLOC 256
#define FACTOR 2

#define safe_assign(var_ptr, val)                                              \
	if (var_ptr != NULL) {                                                     \
		*var_ptr = val;                                                        \
	}

static int recvd_signal;
static void sig_handler(int code);

static void shift_str(
	char *array, uint32_t length, uint32_t index, uint32_t count);
static void unshift_str(
	char *array, uint32_t length, uint32_t index, uint32_t count);

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
	if (!nrl_load_terminfo()) {
		return NULL;
	}
	nrl_dfa_build();

	if (options->fd < 0) {
		safe_assign(err, NRL_ERR_BAD_FD);
		return NULL;
	}
	nrl_io_init(options->fd);

	// Setup terminal options
	struct termios old_attr;
	if (isatty(options->fd)) {
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
	nrl_io_write_esc(TIO_KEYPAD_XMIT);

	// Print prompt:
	nrl_io_write(options->prompt, strlen(options->prompt));
	nrl_io_flush();

	// Input processing
	char *line_buf = malloc(START_ALLOC * sizeof(char));
	uint32_t alloc_length = START_ALLOC;
	uint32_t input_length = 0;
	uint32_t line_cursor = 0;

	nrl_input input;
	input_type res;
	while ((res = nrl_io_read(&input)) != INPUT_STOP) {
		// Newline ends processing
		if (res == INPUT_ASCII && input.ascii == '\n') {
			break;
		}

		uint32_t redraw_start;
		uint32_t redraw_stop;
		int32_t cursor_end_delta = 0;
		int removal = 0;

		if (res == INPUT_ESCAPE) {
			// Escape keys (non-backspace)
			switch (input.escape) {
			case TII_KEY_LEFT:
				if (line_cursor > 0) {
					line_cursor--;
					if (options->echo != NRL_ECHO_NO) {
						nrl_io_write_esc(TIO_CURSOR_LEFT);
					}
				}
				break;
			case TII_KEY_RIGHT:
				if (line_cursor < input_length) {
					line_cursor++;
					if (options->echo != NRL_ECHO_NO) {
						nrl_io_write_esc(TIO_CURSOR_RIGHT);
					}
				}
				break;
			case TII_KEY_HOME:
				for (uint32_t i = line_cursor; i > 0; i--) {
					if (options->echo != NRL_ECHO_NO) {
						nrl_io_write_esc(TIO_CURSOR_LEFT);
					}
				}
				line_cursor = 0;
				break;
			case TII_KEY_END:
				for (uint32_t i = line_cursor; i < input_length; i++) {
					if (options->echo != NRL_ECHO_NO) {
						nrl_io_write_esc(TIO_CURSOR_RIGHT);
					}
				}
				line_cursor = input_length;
				break;
			case TII_KEY_BACKSPACE:
				if (line_cursor == 0) {
					continue;
				}

				unshift_str(line_buf, input_length, line_cursor - 1, 1);
				if (options->echo != NRL_ECHO_NO) {
					nrl_io_write_esc(TIO_CURSOR_LEFT);
				}

				line_buf[input_length - 1] = ' ';
				redraw_start = --line_cursor;
				redraw_stop = input_length--;
				// -1 absolute, but 0 relative to the end
				cursor_end_delta = 0;
				removal = 1;
				break;
			case TII_KEY_DELETE:
				if (line_cursor == input_length) {
					continue;
				}

				unshift_str(line_buf, input_length, line_cursor, 1);

				line_buf[input_length - 1] = ' ';
				redraw_start = line_cursor;
				redraw_stop = input_length--;
				// -1 absolute, but 0 relative to the end
				cursor_end_delta = 0;
				removal = 1;
				break;
			default:
				break;
			}

			// We want to redraw for some, but exit for others
			switch (input.escape) {
			case TII_KEY_BACKSPACE: // fallthrough
			case TII_KEY_DELETE:
				break;
			default:
				nrl_io_flush();
				continue;
			}
		} else {
			// Add characters to buffer
			uint32_t chars_to_add
				= (res == INPUT_SPECIAL) ? strlen(input.special) : 1;

			// Need to increase buffer
			if (input_length + chars_to_add > alloc_length) {
				alloc_length *= FACTOR;
				line_buf = realloc(line_buf, alloc_length * sizeof(char));
			}

			// Shift characters
			if (line_cursor != input_length) {
				shift_str(line_buf, input_length, line_cursor, chars_to_add);
			}

			// Add characters
			if (res == INPUT_SPECIAL) {
				memcpy(line_buf + line_cursor, input.special, chars_to_add);
			} else {
				line_buf[line_cursor] = input.ascii;
			}

			redraw_start = line_cursor;
			line_cursor += chars_to_add;

			input_length += chars_to_add;
			redraw_stop = input_length;

			cursor_end_delta = chars_to_add;
		}

		// Redraw
		uint32_t redraw_length = redraw_stop - redraw_start;
		switch (options->echo) {
		case NRL_ECHO_NO:
			break;
		case NRL_ECHO_YES:
			nrl_io_write(line_buf + redraw_start, redraw_length * sizeof(char));
			break;
		case NRL_ECHO_FAKE:
			for (uint32_t i = 0; i < redraw_length - 1; i++) {
				nrl_io_write(&options->echo_repl, sizeof(char));
			}

			const char *final = removal ? &whitespace : &options->echo_repl;
			nrl_io_write(final, sizeof(char));
			break;
		}

		if (options->echo != NRL_ECHO_NO) {
			for (uint32_t i = 0; i < (redraw_length - cursor_end_delta); i++) {
				nrl_io_write_esc(TIO_CURSOR_LEFT);
			}

			nrl_io_flush();
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

	if (isatty(options->fd)) {
		if (tcsetattr(options->fd, TCSAFLUSH, &old_attr) < 0) {
			safe_assign(err, NRL_ERR_SYS);
			free(line_buf);
			return NULL;
		}
	}

	// Put terminal into local mode
	nrl_io_write_esc(TIO_KEYPAD_LOCAL);

	// Write newline
	char nl_buf = '\n';
	nrl_io_write(&nl_buf, sizeof(char));
	nrl_io_flush();

	// Resend signal to user
	if (res == INPUT_STOP && errno == EINTR) {
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
 * @brief Insert elements into string, shifting elements forward.
 *
 * @param[in,out] array - String.
 * @param[in] length - Current length of string.
 * @param[in] index - Index to insert into.
 * @param[in] count - How many bytes to insert.
 */
static void shift_str(
	char *array, uint32_t length, uint32_t index, uint32_t count) {
	for (uint32_t i = length + count - 1; i >= index + count; i--) {
		array[i] = array[i - count];
	}
}

/**
 * @brief Erase elements in string, shifting elements back.
 *
 * @param[in,out] array - String.
 * @param[in] length - Current length of string.
 * @param[in] index - Index to erase.
 * @param[in] count - How many bytes to erase.
 */
static void unshift_str(
	char *array, uint32_t length, uint32_t index, uint32_t count) {
	for (uint32_t i = index; i < length - count; i++) {
		array[i] = array[i + count];
	}
}
