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

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dfa.h"
#include "terminfo.h"

#define BUF_SIZE 1024

static int fd = -1;

static char rd_buf[BUF_SIZE];
static uint32_t rd_count = 0;
static uint32_t rd_consumed = 0;
static uint32_t rd_pending = 0;

static char wr_buf[BUF_SIZE];
static uint32_t wr_count = 0;

static int io_nextchar(void);

static const char *special_escape = "^[";

void nrl_io_init(int fd_io) {
	fd = fd_io;

	// Reset buffers
	rd_count = 0;
	rd_consumed = 0;
	rd_pending = 0;
	wr_count = 0;
}

input_type nrl_io_read(nrl_input *buffer) {
	// Handle escape characters
	int dfa_res = nrl_dfa_search(&io_nextchar, &buffer->escape);
	if (dfa_res < 0) {
		return INPUT_STOP;
	}

	if (dfa_res == 1) {
		rd_consumed += rd_pending;
		rd_pending = 0;
		return INPUT_ESCAPE;
	}

	char ascii = rd_buf[rd_consumed];
	rd_consumed++;
	rd_pending = 0;

	// Handle unprintables
	if (ascii == '\e') {
		buffer->special = special_escape;
		return INPUT_SPECIAL;
	}

	buffer->ascii = ascii;
	return INPUT_ASCII;
}

void nrl_io_write(const char *data, uint32_t length) {
	if (length > BUF_SIZE) {
		write(fd, data, length);
		return;
	}

	if (wr_count + length > BUF_SIZE) {
		nrl_io_flush();
	}
	memcpy(wr_buf + wr_count, data, length);
	wr_count += length;
}

void nrl_io_write_esc(terminfo_output sequence) {
	const char *as_text = nrl_lookup_output(sequence);
	nrl_io_write(as_text, strlen(as_text));
}

void nrl_io_flush(void) {
	if (wr_count == 0) {
		return;
	}

	write(fd, wr_buf, wr_count);
	wr_count = 0;
}

/**
 * @brief Get next character from input buffer.
 *
 * @return ASCII char if available, '\0' if no input and EOF on error.
 */
static int io_nextchar(void) {
	// No characters - read in data
	if (rd_consumed == rd_count) {
		ssize_t bytes = read(fd, rd_buf, BUF_SIZE);
		if (bytes <= 0) {
			return EOF;
		}

		rd_count = bytes;
		rd_consumed = 0;
		rd_pending = 0;
	}

	// We hit end of buffer, but not done with DFA parse
	// Assume that a sequence is negligible in size, relative to the size of
	// the buffer
	if (rd_consumed + rd_pending == rd_count) {
		// Move pending bits to the start of the array
		if (rd_pending <= rd_consumed) {
			memcpy(rd_buf, rd_buf + rd_consumed, rd_pending);
		} else {
			for (uint32_t i = 0; i < rd_pending; i++) {
				rd_buf[i] = rd_buf[rd_consumed + i];
			}
		}

		rd_count = rd_pending;
		rd_consumed = 0;

		// Read in more characters (but only if they are available)
		ssize_t bytes = read(fd, rd_buf + rd_count, BUF_SIZE - rd_count);
		rd_count += bytes;
	}

	char next_char = rd_buf[rd_consumed + rd_pending++];

	// Ctrl+D / EOF
	if (next_char == '\4') {
		return EOF;
	}

	return next_char;
}
