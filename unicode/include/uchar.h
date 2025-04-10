/**
 * @file uchar.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version dev
 * @date 2025
 * @license LGPLv3.0
 * @brief Unicode character type and UTF-8 conversion.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t uchar;

/**
 * @brief Decode a UTF-8 encoded C-string into internal representation.
 *
 * @param[in] utf8_str - UTF-8 encoded string.
 * @param[out] error_flag - Indicates if errors were encountered and replaced
 * with the replacement character (can be NULL).
 * @return Unicode string.
 */
uchar *utf8_parse(const char *utf8_str, bool *error_flag);

/**
 * @brief Create a UTF-8 encoded C-string from the internal representation.
 *
 * @param[in] ustr - Unicode string.
 * @return UTF-8 encoded string.
 */
char *utf8_encode(const uchar *ustr);
