/**
 * @file io.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.1
 * @date 2024
 * @license LGPLv3.0
 * @brief Input and output processing.
 */
#pragma once

#include <stdint.h>

#include "terminfo.h"

typedef union {
	char ascii;
	terminfo_entry escape;
} nrl_input;

/**
 * @brief Initialize IO buffers and set source.
 *
 * @param[in] fd - File descriptor.
 */
void nrl_io_init(int fd);

/**
 * @brief Get the next character from input.
 *
 * @param[out] buffer - Input value buffer.
 * @return 0 if regular character, 1 if escape character.
 * @note Will block if not data in buffer.
 */
int nrl_io_read(nrl_input *buffer);

/**
 * @brief Queue data to print.
 *
 * @param[in] data - Data buffer.
 * @param[in] length - Data length.
 * @note Will flush on buffer fill.
 */
void nrl_io_write(const char *data, uint32_t length);

/**
 * @brief Queue escape sequence to print.
 *
 * @param[in] sequence - Escape sequence identifier.
 * @note Will flush on buffer fill.
 */
void nrl_io_write_esc(terminfo_entry sequence);

/**
 * @brief Flush buffer to output.
 */
void nrl_io_flush(void);
