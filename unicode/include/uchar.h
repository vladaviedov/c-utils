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

typedef enum {
	U8BI_INVALID = -1,
	U8BI_CONT,
	U8BI_ASCII,
	U8BI_2BYTES,
	U8BI_3BYTES,
	U8BI_4BYTES,
} utf8_byte_info;

/**
 * @brief Inspect a single byte from a UTF-8 stream.
 *
 * @param[in] utf8_byte - Byte.
 * @return Character type.
 */
utf8_byte_info utf8_inspect(const char utf8_byte);

/**
 * @brief Parse a single unicode point from a UTF-8 string.
 *
 * @param[in] s - UTF-8 string.
 * @param[out] buffer - Buffer to write the parsed unicode point.
 * @return 0 - Invalid UTF-8 sequence. \\
 *         n - Amount of bytes consumed from 's'.
 */
uint32_t utf8_parse_uchar(const char *str, uchar *buffer);

/**
 * @brief Decode a UTF-8 encoded C-string into internal representation.
 *
 * @param[in] utf8_str - UTF-8 encoded string.
 * @param[out] error_flag - Indicates if errors were encountered and replaced
 * with the replacement character (can be NULL).
 * @return Unicode string.
 */
uchar *utf8_decode(const char *utf8_str, bool *error_flag);

/**
 * @brief Create a UTF-8 encoded C-string from the internal representation.
 *
 * @param[in] ustr - Unicode string.
 * @return UTF-8 encoded string.
 */
char *utf8_encode(const uchar *ustr);
