/**
 * @file uchar.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.0
 * @date 2025
 * @license LGPLv3.0
 * @brief Unicode character type and UTF-8 conversion.
 */
#include "uchar.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// UTF-8 limits
#define MAX_1BYTE 0x0000007f
#define MAX_2BYTE 0x000007ff
#define MAX_3BYTE 0x0000ffff
#define MAX_4BYTE 0x0010ffff

// Continuation byte description
#define CONT_HEAD 0b10000000
#define CONT_HEAD_MASK 0b11000000
#define CONT_DATA_MASK 0b00111111
#define CONT_BITS 6

// Leading byte description
#define LEAD_2BYTE_HEAD 0b11000000
#define LEAD_2BYTE_HEAD_MASK 0b11100000
#define LEAD_2BYTE_DATA_MASK 0b00011111

#define LEAD_3BYTE_HEAD 0b11100000
#define LEAD_3BYTE_HEAD_MASK 0b11110000
#define LEAD_3BYTE_DATA_MASK 0b00001111

#define LEAD_4BYTE_HEAD 0b11110000
#define LEAD_4BYTE_HEAD_MASK 0b11111000
#define LEAD_4BYTE_DATA_MASK 0b00000111

// Error replacement character
// Encouraged for use instead of crashing the parser
#define REPLACEMENT_CHAR 0xfffd

#define safe_assign(var_ptr, val)                                              \
	if (var_ptr != NULL) {                                                     \
		*var_ptr = val;                                                        \
	}

static uint64_t calc_utf8_length(const uchar *ustr);
static uint64_t try_calc_uchar_length(const char *str);
static uint32_t encode_point(uchar c, char *buffer);

utf8_byte_info utf8_inspect(char utf8_byte) {
	uint8_t as_int = (uint8_t)utf8_byte;

	// Check for ASCII first, it's the most common
	if (as_int <= 0x7f) {
		return U8BI_ASCII;
	}

	// Check for invalid codes
	if (as_int == 0xc0 || as_int == 0xc1 || as_int >= 0xf5) {
		return U8BI_INVALID;
	}

	// Multibyte bytes
	if ((as_int & CONT_HEAD_MASK) == CONT_HEAD) {
		return U8BI_CONT;
	} else if ((as_int & LEAD_2BYTE_HEAD_MASK) == LEAD_2BYTE_HEAD) {
		return U8BI_2BYTES;
	} else if ((as_int & LEAD_3BYTE_HEAD_MASK) == LEAD_3BYTE_HEAD) {
		return U8BI_3BYTES;
	} else {
		return U8BI_4BYTES;
	}
}

uint32_t utf8_parse_uchar(const char *str, uchar *buffer) {
	if (str == NULL) {
		return 0;
	}

	char leader = str[0];

	uint32_t size;
	char leader_data;

	utf8_byte_info leader_info = utf8_inspect(leader);
	switch (leader_info) {
	case U8BI_ASCII:
		*buffer = leader;
		return 1;
	case U8BI_2BYTES:
		leader_data = leader & LEAD_2BYTE_DATA_MASK;
		size = 2;
		break;
	case U8BI_3BYTES:
		leader_data = leader & LEAD_3BYTE_DATA_MASK;
		size = 3;
		break;
	case U8BI_4BYTES:
		leader_data = leader & LEAD_4BYTE_DATA_MASK;
		size = 4;
		break;
	case U8BI_CONT: // fallthrough
	case U8BI_INVALID:
		return 0;
	}

	// Write leader data bits
	*buffer = leader_data << ((size - 1) * CONT_BITS);

	// Parse continuation bits
	for (uint32_t i = 1; i < size; i++) {
		char raw = str[i];

		// Check for continuation bit errors
		// Implicitely checks for zero char
		if (utf8_inspect(raw) != U8BI_CONT) {
			return 0;
		}

		char data = raw & CONT_DATA_MASK;
		*buffer |= data << ((size - i - 1) * CONT_BITS);
	}

	// Check for overlong encoding
	if (leader_info == U8BI_2BYTES && *buffer <= MAX_1BYTE) {
		return 0;
	}
	if (leader_info == U8BI_3BYTES && *buffer <= MAX_2BYTE) {
		return 0;
	}
	if (leader_info == U8BI_4BYTES && *buffer <= MAX_3BYTE) {
		return 0;
	}

	// Check for point being too large
	if (*buffer > MAX_4BYTE) {
		return 0;
	}

	return size;
}

uchar *utf8_decode(const char *utf8_str, bool *error_flag) {
	if (utf8_str == NULL) {
		safe_assign(error_flag, true);
		return NULL;
	}

	// Fallback unicode string size
	uint64_t fallback_size = strlen(utf8_str) + 1;

	// Pre-calculate unicode string size
	uint64_t uc_size = try_calc_uchar_length(utf8_str);
	if (uc_size == 0) {
		uc_size = fallback_size;
	}
	uchar *uc_str = malloc(uc_size * sizeof(uchar));

	// Parse points
	safe_assign(error_flag, false);
	uint64_t ptr = 0;
	bool null_reached = false;
	do {
		// If the size calculation was wrong
		if (ptr >= uc_size) {
			uc_size = fallback_size;
			uc_str = realloc(uc_str, fallback_size);
		}

		null_reached = (*utf8_str == '\0');
		uint32_t consumed = utf8_parse_uchar(utf8_str, uc_str + ptr);
		if (consumed == 0) {
			// Parse error
			safe_assign(error_flag, true);
			uc_str[ptr] = REPLACEMENT_CHAR;
			utf8_str++;
		} else {
			utf8_str += consumed;
		}

		ptr++;
	} while (!null_reached);

	return uc_str;
}

char *utf8_encode(const uchar *ustr) {
	if (ustr == NULL) {
		return NULL;
	}

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
 * @return 0 - Impossible to encode as UTF-8.\n
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
 * @brief Try to calculate the amount of unicode points that the string will
 * produce. This function is only guaranteed to produce the correct length for
 * valid UTF-8 strings.
 *
 * @param[in] str - UTF-8 string.
 * @return 0 - Failed due to errors in the string.\n
 *         n - Amount of unicode points the string *should* occupy.
 */
static uint64_t try_calc_uchar_length(const char *str) {
	uint64_t uc_size = 0;

	uint64_t ptr = 0;
	uint64_t str_size = strlen(str);

	char c;
	while ((c = str[ptr]) != '\0') {
		switch (utf8_inspect(c)) {
		case U8BI_ASCII:
			ptr += 1;
			break;
		case U8BI_2BYTES:
			ptr += 2;
			break;
		case U8BI_3BYTES:
			ptr += 3;
			break;
		case U8BI_4BYTES:
			ptr += 4;
			break;
		case U8BI_CONT: // fallthrough
		case U8BI_INVALID:
			return 0;
		}

		// We have overrun the end of the string
		if (ptr > str_size) {
			return 0;
		}

		uc_size++;
	}

	// With null-terminator
	return uc_size + 1;
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
		char byte1_data = c & CONT_DATA_MASK;

		buffer[0] = LEAD_2BYTE_HEAD | byte0_data;
		buffer[1] = CONT_HEAD | byte1_data;

		return 2;
	}

	// 3 bytes
	if (c <= MAX_3BYTE) {
		char byte0_data = c >> (2 * CONT_BITS);
		char byte1_data = (c >> CONT_BITS) & CONT_DATA_MASK;
		char byte2_data = c & CONT_DATA_MASK;

		buffer[0] = LEAD_3BYTE_HEAD | byte0_data;
		buffer[1] = CONT_HEAD | byte1_data;
		buffer[2] = CONT_HEAD | byte2_data;

		return 3;
	}

	// 4 bytes
	char byte0_data = c >> (3 * CONT_BITS);
	char byte1_data = (c >> (2 * CONT_BITS)) & CONT_DATA_MASK;
	char byte2_data = (c >> CONT_BITS) & CONT_DATA_MASK;
	char byte3_data = c & CONT_DATA_MASK;

	buffer[0] = LEAD_4BYTE_HEAD | byte0_data;
	buffer[1] = CONT_HEAD | byte1_data;
	buffer[2] = CONT_HEAD | byte2_data;
	buffer[3] = CONT_HEAD | byte3_data;

	return 4;
}
