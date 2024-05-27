#pragma once

#include <stdio.h>
#include <termios.h>

typedef enum {
	NRL_OK = 0,
	NRL_EMPTY,
	NRL_BAD_FD,
	NRL_SYS_ERR,
} nrl_error;

typedef enum {
	NRL_NO_ECHO,
	NRL_ECHO,
	NRL_FAKE_ECHO,
} nrl_echo_mode;

typedef struct {
	int fd;
	const char *prompt;
	nrl_echo_mode echo;
	char echo_repl;
} nrl_opts;

char *nanorl(const char *prompt, nrl_error *err);

char *nanorl_fd(int fd, const char *prompt, nrl_error *err);

char *nanorl_adv(const nrl_opts *options, nrl_error *err);
