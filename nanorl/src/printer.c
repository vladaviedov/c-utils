/**
 * @file printer.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.0
 * @date 2024
 * @license LGPLv3.0
 * @brief Buffered terminal printing.
 */
#define _POSIX_C_SOURCE 200809L
#include "printer.h"

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "terminfo.h"

#define BUF_SIZE 1024

static int print_fd = -1;

static char write_buf[BUF_SIZE];
static uint32_t byte_count = 0;

void nrl_set_fd(int fd) {
	print_fd = fd;
}

void nrl_write(const char *data, uint32_t length) {
	if (length > BUF_SIZE) {
		write(print_fd, data, length);
		return;
	}

	if (byte_count + length > BUF_SIZE) {
		nrl_flush();
	}
	memcpy(write_buf + byte_count, data, length);
	byte_count += length;
}

void nrl_write_esc(terminfo_entry sequence) {
	const char *as_text = nrl_lookup_seq(sequence);
	nrl_write(as_text, strlen(as_text));
}

void nrl_flush(void) {
	if (byte_count == 0) {
		return;
	}

	write(print_fd, write_buf, byte_count);
	byte_count = 0;
}
