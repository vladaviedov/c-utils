#define _POSIX_C_SOURCE 200112L
#include "nanorl.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#define START_ALLOC 1024
#define FACTOR 2

#define safe_assign(var_ptr, val) \
	if (var_ptr != NULL) { \
		*var_ptr = val; \
	}

char *nanorl(const char *prompt, nrl_error *err) {
	return nanorl_fd(STDIN_FILENO, prompt, err);
}

char *nanorl_file(FILE *source, const char *prompt, nrl_error *err) {
	return nanorl_fd(fileno(source), prompt, err);
}

char *nanorl_fd(int fd, const char *prompt, nrl_error *err) {
	if (fd < 0) {
		safe_assign(err, NRL_BAD_FD);
		return NULL;
	}
	
	// Setup terminal options
	struct termios old_attr;
	if (tcgetattr(fd, &old_attr) < 0) {
		safe_assign(err, NRL_SYS_ERR);
		return NULL;
	}

	struct termios new_attr = old_attr;
	new_attr.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(fd, TCSAFLUSH, &new_attr) < 0) {
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
	new_sa.sa_handler = NULL;

	if (sigaction(SIGINT, &new_sa, &old_sigint_sa) < 0
		|| sigaction(SIGTERM, &new_sa, &old_sigterm_sa) < 0
		|| sigaction(SIGQUIT, &new_sa, &old_sigquit_sa) < 0) {
		safe_assign(err, NRL_SYS_ERR);
		return NULL;
	}

	// Print prompt:
	write(fd, prompt, strlen(prompt) + 1);

	// Input processing
	// TODO: handle non-alpha
	char *input_buf = malloc(sizeof(char) * 1024);
	uint32_t alloc_length = 1024;
	uint32_t input_length = 0;
	char input_ch;
	while (read(fd, &input_ch, sizeof(char)) > 0 && input_ch != '\n') {
		if (input_length == alloc_length) {
			input_buf = realloc(input_buf, alloc_length * FACTOR);
		}

		input_buf[input_length] = input_ch;
		input_length++;
		write(fd, &input_ch, 1);
	}

	// Reset terminal settings
	if (sigaction(SIGINT, &old_sigint_sa, NULL) < 0
		|| sigaction(SIGTERM, &old_sigterm_sa, NULL) < 0
		|| sigaction(SIGQUIT, &old_sigquit_sa, NULL) < 0) {
		safe_assign(err, NRL_SYS_ERR);
		free(input_buf);
		return NULL;
	}

	if (tcsetattr(fd, TCSAFLUSH, &old_attr) < 0) {
		safe_assign(err, NRL_SYS_ERR);
		free(input_buf);
		return NULL;
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
