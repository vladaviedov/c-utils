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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "terminfo.h"
#include "dfa.h"

#define BUF_SIZE 1024

static int fd = -1;

static char rd_buf[BUF_SIZE];
static uint32_t rd_count = 0;
static uint32_t rd_consumed = 0;
static uint32_t rd_pending = 0;

static char wr_buf[BUF_SIZE];
static uint32_t wr_count = 0;

static int io_nextchar();

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

void nrl_io_write_esc(terminfo_entry sequence) {
	const char *as_text = nrl_lookup_seq(sequence);
	nrl_io_write(as_text, strlen(as_text));
}

void nrl_io_flush(void) {
	if (wr_count == 0) {
		return;
	}

	write(fd, wr_buf, wr_count);
	wr_count = 0;
}

static int io_nextchar() {
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

	// TODO: handle when pending hits length properly
	if (rd_consumed + rd_pending == rd_count) {
		return '\0';
	}
	
	return rd_buf[rd_consumed + rd_pending++];
}
