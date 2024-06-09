/**
 * @file printer.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version pre1.0
 * @date 2024
 * @license LGPLv3.0
 * @brief Buffered terminal printing.
 */
#pragma once

#include <stdint.h>

#include "terminfo.h"

/**
 * @brief Select file descriptor for printing.
 *
 * @param[in] fd - File descriptor.
 */
void nrl_set_fd(int fd);

/**
 * @brief Queue data to print.
 *
 * @param[in] data - Data buffer.
 * @param[in] length - Data length.
 * @note Will flush on buffer fill.
 */
void nrl_write(const char *data, uint32_t length);

/**
 * @brief Queue escape sequence to print.
 *
 * @param[in] sequence - Escape sequence identifier.
 * @note Will flush on buffer fill.
 */
void nrl_write_esc(terminfo_entry sequence);

/**
 * @brief Flush buffer to output.
 */
void nrl_flush(void);
