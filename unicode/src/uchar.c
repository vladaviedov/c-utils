/**
 * @file uchar.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version dev
 * @date 2025
 * @license LGPLv3.0
 * @brief Unicode character type and UTF-8 conversion.
 */
#include "uchar.h"

#include <stdint.h>
#include <stdlib.h>

#define MAX_1BYTE 0x0000007f
#define MAX_2BYTE 0x000007ff
#define MAX_3BYTE 0x0000ffff
#define MAX_4BYTE 0x0010ffff

#define CONT_HEAD 0b1000000
#define CONT_MASK 0b00111111
#define CONT_BITS 6

#define LEAD_2BYTE_HEAD 0b11000000
#define LEAD_2BYTE_MASK 0b00011111

#define LEAD_3BYTE_HEAD 0b11100000
#define LEAD_3BYTE_MASK 0b00001111

#define LEAD_4BYTE_HEAD 0b11110000
#define LEAD_4BYTE_MASK 0b00000111

static uint64_t calc_utf8_length(const uchar *ustr);
static uint32_t encode_point(uchar c, char *buffer);

uchar *utf8_parse(const char *utf8_str) {
	// TODO: implement
	return NULL;
}

char *utf8_encode(const uchar *ustr) {
	// Pre-calculate UTF-8 string size
	uint64_t utf8_size = calc_utf8_length(ustr);
	if (utf8_size == 0) {
		return NULL;
	}
	char *utf8_str = malloc(utf8_size * sizeof(char));

	// Encode unicode points with null-terminator
	uint64_t ptr = 0;
	do {
		ptr += encode_point(*ustr, utf8_str + ptr);
	} while (*ustr++ != 0);

	return utf8_str;
}

/**
 * @brief Calculate the amount of bytes required to encode the unicode string as
 * a UTF-8 string.
 *
 * @param[in] ustr - Unicode string.
 * @return 0 - Impossible to encode as UTF-8. \\
 *         n - Size required including the null-terminator.
 */
static uint64_t calc_utf8_length(const uchar *ustr) {
	uint64_t size = 0;

	uchar c;
	while ((c = *ustr++) != 0) {
		if (c <= MAX_1BYTE) {
			size += 1;
		} else if (c <= MAX_2BYTE) {
			size += 2;
		} else if (c <= MAX_3BYTE) {
			size += 3;
		} else if (c <= MAX_4BYTE) {
			size += 4;
		} else {
			// Cannot encode as UTF-8
			return 0;
		}
	}

	// With null-terminator
	return size + 1;
}

/**
 * @brief Encode a single unicode point as UTF-8 into a buffer.
 *
 * @param[in] c - Unicode point to encode.
 * @param[out] buffer - Buffer for writing characters.
 * @return Amount of characters written into 'buffer'.
 */
static uint32_t encode_point(uchar c, char *buffer) {
	// 1 byte
	if (c <= MAX_1BYTE) {
		buffer[0] = c;
		return 1;
	}

	// 2 bytes
	if (c <= MAX_2BYTE) {
		char byte0_data = c >> CONT_BITS;
		char byte1_data = c & CONT_MASK;

		buffer[0] = LEAD_2BYTE_HEAD | byte0_data;
		buffer[1] = CONT_HEAD | byte1_data;

		return 2;
	}

	// 3 bytes
	if (c <= MAX_3BYTE) {
		char byte0_data = c >> (2 * CONT_BITS);
		char byte1_data = (c >> CONT_BITS) & CONT_MASK;
		char byte2_data = c & CONT_MASK;

		buffer[0] = LEAD_3BYTE_HEAD | byte0_data;
		buffer[1] = CONT_HEAD | byte1_data;
		buffer[2] = CONT_HEAD | byte2_data;

		return 3;
	}

	// 4 bytes
	char byte0_data = c >> (3 * CONT_BITS);
	char byte1_data = (c >> (2 * CONT_BITS)) & CONT_MASK;
	char byte2_data = (c >> CONT_BITS) & CONT_MASK;
	char byte3_data = c & CONT_MASK;

	buffer[0] = LEAD_4BYTE_HEAD | byte0_data;
	buffer[1] = CONT_HEAD | byte1_data;
	buffer[2] = CONT_HEAD | byte2_data;
	buffer[3] = CONT_HEAD | byte3_data;

	return 4;
}
