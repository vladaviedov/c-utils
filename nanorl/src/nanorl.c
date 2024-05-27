#define _POSIX_C_SOURCE 200112L
#include "nanorl.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#define START_ALLOC 1024
#define FACTOR 2

#define safe_assign(var_ptr, val) \
	if (var_ptr != NULL) { \
		*var_ptr = val; \
	}

static int recvd_signal;

static void sig_handler(int code);

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
	// TODO: handle non-alpha
	char *input_buf = malloc(sizeof(char) * 1024);
	uint32_t alloc_length = 1024;
	uint32_t input_length = 0;

	ssize_t res;
	char input_ch;
	while ((res = read(options->fd, &input_ch, sizeof(char))) > 0 && input_ch != '\n') {
		if (input_length == alloc_length) {
			input_buf = realloc(input_buf, alloc_length * FACTOR);
		}

		input_buf[input_length] = input_ch;
		input_length++;

		switch (options->echo) {
		case NRL_NO_ECHO:
			break;
		case NRL_ECHO:
			write(options->fd, &input_ch, 1);
			break;
		case NRL_FAKE_ECHO:
			write(options->fd, &options->echo_repl, 1);
			break;
		}
	}

	// Reset terminal settings
	if (sigaction(SIGINT, &old_sigint_sa, NULL) < 0
		|| sigaction(SIGTERM, &old_sigterm_sa, NULL) < 0
		|| sigaction(SIGQUIT, &old_sigquit_sa, NULL) < 0) {
		safe_assign(err, NRL_SYS_ERR);
		free(input_buf);
		return NULL;
	}

	if (tcsetattr(options->fd, TCSAFLUSH, &old_attr) < 0) {
		safe_assign(err, NRL_SYS_ERR);
		free(input_buf);
		return NULL;
	}

	// Resend signal to user
	if (res < 0 && errno == EINTR) {
		raise(recvd_signal);
	}

	// Exit
	if (input_length == 0) {
		safe_assign(err, NRL_EMPTY);
		free(input_buf);
		return NULL;
	}

	safe_assign(err, NRL_OK);
	return input_buf;
}

static void sig_handler(int code) {
	recvd_signal = code;
}
