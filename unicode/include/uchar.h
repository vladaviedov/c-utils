/**
 * @file uchar.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version dev
 * @date 2025
 * @license LGPLv3.0
 * @brief Unicode base and encodings.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t uchar;

/**
 * @enum utf8_byte_info
 * Byte type inside a UTF-8 string.
 *
 * @var utf8_byte_info::U8BI_INVALID
 * Invalid byte in UTF-8.
 *
 * @var utf8_byte_info::U8BI_CONT
 * Continunation byte of a multibyte unicode point.
 *
 * @var utf8_byte_info::U8BI_ASCII
 * Single byte ASCII-equivalent character.
 *
 * @var utf8_byte_info::U8BI_2BYTES
 * Leading byte of a 2-byte unicode point.
 *
 * @var utf8_byte_info::U8BI_3BYTES
 * Leading byte of a 3-byte unicode point.
 *
 * @var utf8_byte_info::U8BI_4BYTES
 * Leading byte of a 4-byte unicode point.
 */
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
utf8_byte_info utf8_inspect(char utf8_byte);

/**
 * @brief Parse a single unicode point from a UTF-8 string.
 *
 * @param[in] str - UTF-8 string.
 * @param[out] buffer - Buffer to write the parsed unicode point.
 * @return 0 - Invalid UTF-8 sequence.\n
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
