#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <c-utils/nanorl.h>

int main(void) {
	// Basic usage
	char *input = nanorl("enter something: ", NULL);
	printf("You typed: %s\n\n", input);
	free(input);

	// Use another fd
	int tty = open("/dev/tty", O_RDWR);
	input = nanorl_fd(tty, "enter from tty: ", NULL);
	printf("TTY typed: %s\n\n", input);
	if (input != NULL) {
		free(input);
	}

	// Take in a password
	nrl_opts options = {
		.fd = tty,
		.prompt = "input password: ",
		.echo = NRL_ECHO_FAKE,
		.echo_repl = '*',
	};
	input = nanorl_opts(&options, NULL);
	printf("Your password is '%s' :)\n\n", input);
	if (input != NULL) {
		memset(input, 0, strlen(input) + 1);
		free(input);
	}

	// Or alternatively have it fully hidden
	options.echo = NRL_ECHO_NO;
	input = nanorl_opts(&options, NULL);
	printf("Your password is '%s' :)\n\n", input);
	if (input != NULL) {
		memset(input, 0, strlen(input) + 1);
		free(input);
	}

	// Handle library errors
	nrl_error err;
	input = nanorl_fd(-1, "cmon do somthing: ", &err);
	if (err == NRL_ERR_BAD_FD) {
		printf("bad fd as expected!\n");
	}

	close(tty);
	return 0;
}
