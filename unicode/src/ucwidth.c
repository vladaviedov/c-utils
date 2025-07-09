/**
 * @file ucwidth.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version dev
 * @date 2025
 * @license LGPLv3.0
 * @brief Unicode character width.
 */
#include "ucwidth.h"

#include <stdbool.h>
#include <stdint.h>

#include "uchar.h"

// C0 control characters
#define CTRL_C0_MIN 0x01
#define CTRL_C0_MAX 0x1f
// C1 control characters (plus delete)
#define CTRL_C1_MIN 0x7f
#define CTRL_C1_MAX 0x9f

typedef struct {
	uchar start;
	uchar end;
} unicode_range;

// Table of East Asian full width ranges
static const unicode_range fullwidth[] = {
#include "../gen/fullwidth.gen"
};
static const uint64_t fullwidth_len = sizeof(fullwidth) / sizeof(unicode_range);

// Table of zero width ranges
static const unicode_range zerowidth[] = {
#include "../gen/zerowidth.gen"
};
static const uint64_t zerowidth_len = sizeof(zerowidth) / sizeof(unicode_range);

static bool is_fullwidth(uchar uc);
static bool is_zerowidth(uchar uc);
static bool table_search(
	const unicode_range *table, uint64_t table_len, uchar uc);

int ucwidth(uchar uc) {
	if (uc == 0) {
		return 0;
	}

	// Control ranges
	if ((uc >= CTRL_C0_MIN && uc <= CTRL_C0_MAX)
		|| (uc >= CTRL_C1_MIN && uc <= CTRL_C1_MAX)) {
		return -1;
	}

	// Table search
	if (is_fullwidth(uc)) {
		return 2;
	} else if (is_zerowidth(uc)) {
		return 0;
	} else {
		return 1;
	}
}

int ucswidth(const uchar *s, size_t n) {
	int total_size = 0;
	uchar uc;

	while ((uc = *s++) != 0 && n-- != 0) {
		int uc_size = ucwidth(uc);
		if (uc_size < 0) {
			return -1;
		}

		total_size += uc_size;
	}

	return total_size;
}

/**
 * @brief Check if unicode character is East Asian full width.
 *
 * @param[in] uc - Unicode character point.
 * @return True if character is full width.
 */
static bool is_fullwidth(uchar uc) {
	return table_search(fullwidth, fullwidth_len, uc);
}

/**
 * @brief Check if unicode character is zero width.
 *
 * @param[in] uc - Unicode character point.
 * @return True if character is zero width.
 */
static bool is_zerowidth(uchar uc) {
	return table_search(zerowidth, zerowidth_len, uc);
}

/**
 * @brief Perform binary search on a range table.
 *
 * @param[in] table - Unicode range table.
 * @param[in] table_len - Table length.
 * @param[in] uc - Unicode character point.
 * @return True if character is found.
 */
static bool table_search(
	const unicode_range *table, uint64_t table_len, uchar uc) {
	// Table index bounds
	uint32_t min = 0;
	uint32_t max = table_len - 1;

	// Outside bounds must be half-width
	if (uc < table[min].start || uc > table[max].end) {
		return false;
	}

	// Perform binary search
	while (min <= max) {
		uchar mid = (min + max) / 2;
		const unicode_range *range = &table[mid];

		if (uc > range->end) {
			min = mid + 1;
		} else if (uc < range->start) {
			max = mid - 1;
		} else {
			// Point is within current range
			return true;
		}
	}

	return false;
}
