/**
 * @file ustring.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version dev
 * @date 2025
 * @license LGPLv3.0
 * @brief string.h equivalent for unicode point strings.
 */
#include "ustring.h"

#include <stdlib.h>
#include <string.h>

#include "uchar.h"

uchar *ustrcpy(uchar *restrict dst, const uchar *restrict src) {
	uchar *restrict trav = dst;

	do {
		*trav++ = *src;
	} while (*src++ != 0);

	return dst;
}

uchar *ustrncpy(uchar *restrict dst, const uchar *restrict src, size_t dsize) {
	if (dsize == 0) {
		return dst;
	}

	uchar *restrict trav = dst;

	do {
		*trav++ = *src;
	} while (--dsize != 0 && *src++ != 0);

	return dst;
}

uchar *ustrdup(const uchar *s) {
	// Account for null-terminator
	size_t count = ustrlen(s) + 1;

	uchar *dup_s = malloc(count * sizeof(uchar));
	if (dup_s == NULL) {
		return NULL;
	}

	memcpy(dup_s, s, count * sizeof(uchar));

	return dup_s;
}

uchar *ustrndup(const uchar *s, size_t n) {
	// Account for null-terminator
	size_t count = ustrnlen(s, n) + 1;

	uchar *dup_s = malloc(count * sizeof(uchar));
	if (dup_s == NULL) {
		return NULL;
	}

	// Copy one less and add null-terminator
	memcpy(dup_s, s, (count - 1) * sizeof(uchar));
	dup_s[count - 1] = 0;

	return dup_s;
}

size_t ustrlen(const uchar *ustr) {
	size_t count = 0;
	while (*ustr++ != 0) {
		count++;
	}

	return count;
}

size_t ustrnlen(const uchar *ustr, size_t maxlen) {
	size_t count = 0;
	while (count < maxlen && *ustr++ != 0) {
		count++;
	}

	return count;
}
