/**
 * @file ucwidth.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version dev
 * @date 2025
 * @license LGPLv3.0
 * @brief Unicode character width.
 */
#include "ucwidth.h"

#include <stdint.h>

#include "uchar.h"

typedef struct {
	uint32_t start;
	uint32_t end;
} unicode_range;

static const unicode_range fullwidth[] = {
	#include "../gen/fullwidth.gen"
};
static const uint64_t fullwidth_len = sizeof(fullwidth) / sizeof(unicode_range);

int ucwidth(uchar uc) {
	return fullwidth_len;
}

int ucswidth(const uchar *s, size_t n) {
	return fullwidth_len;
}
