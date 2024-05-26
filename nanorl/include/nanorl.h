#pragma once

#include <stdio.h>
#include <termios.h>

typedef enum {
	NRL_OK = 0,
	NRL_EMPTY,
	NRL_BAD_FD,
	NRL_SYS_ERR,
} nrl_error;

char *nanorl(const char *prompt, nrl_error *err);

char *nanorl_file(FILE *source, const char *prompt, nrl_error *err);

char *nanorl_fd(int fd, const char *prompt, nrl_error *err);
