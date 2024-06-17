/**
 * @file io.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.1
 * @date 2024
 * @license LGPLv3.0
 * @brief Input and output processing.
 */
#define _POSIX_C_SOURCE 200809L
#include "io.h"

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "terminfo.h"

#define BUF_SIZE 1024

static int print_fd = -1;

static char rd_buf[BUF_SIZE];
static uint32_t rd_count = 0;

static char wr_buf[BUF_SIZE];
static uint32_t wr_count = 0;

void nrl_io_set_fd(int fd) {
	print_fd = fd;

	// Reset buffers
	rd_count = 0;
	wr_count = 0;
}

	// TODO: implement
int nrl_io_read(nrl_input *buffer) {
	return 0;
}

void nrl_io_write(const char *data, uint32_t length) {
	if (length > BUF_SIZE) {
		write(print_fd, data, length);
		return;
	}

	if (wr_count + length > BUF_SIZE) {
		nrl_io_flush();
	}
	memcpy(wr_buf + wr_count, data, length);
	wr_count += length;
}

void nrl_io_write_esc(terminfo_entry sequence) {
	const char *as_text = nrl_lookup_seq(sequence);
	nrl_io_write(as_text, strlen(as_text));
}

void nrl_io_flush(void) {
	if (wr_count == 0) {
		return;
	}

	write(print_fd, wr_buf, wr_count);
	wr_count = 0;
}
