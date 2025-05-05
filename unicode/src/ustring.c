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

	while (dsize-- != 0) {
		*trav++ = 0;
	}

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

uchar *ustrcat(uchar *restrict dst, const uchar *restrict src) {
	uchar *restrict trav = dst;

	// Locate end of 'dst' string
	while (*trav != 0) {
		trav++;
	}

	ustrcpy(trav, src);
	return dst;
}

uchar *ustrncat(uchar *restrict dst, const uchar *restrict src, size_t ssize) {
	uchar *restrict trav = dst;

	// Locate end of 'dst' string
	while (*trav != 0) {
		trav++;
	}

	ustrncpy(trav, src, ssize);

	// Add null-terminator just in case strncpy didn't
	trav[ssize] = 0;

	return dst;
}

int ustrcmp(const uchar *s1, const uchar *s2) {
	while (*s1 != '\0' || *s2 != '\0') {
		if (*s1 == *s2) {
			s1++;
			s2++;
			continue;
		}

		if (*s1 > *s2) {
			return 1;
		} else {
			return -1;
		}
	}

	return 0;
}

int ustrncmp(const uchar *s1, const uchar *s2, size_t ssize) {
	while ((*s1 != '\0' || *s2 != '\0') && ssize != 0) {
		if (*s1 == *s2) {
			s1++;
			s2++;
			ssize--;

			continue;
		}

		if (*s1 > *s2) {
			return 1;
		} else {
			return -1;
		}
	}

	return 0;
}

uchar *ustpcpy(uchar *restrict dst, const uchar *restrict src) {
	do {
		*dst++ = *src;
	} while (*src++ != 0);

	// Back track to null-term byte
	return dst - 1;
}

uchar *ustpncpy(uchar *restrict dst, const uchar *restrict src, size_t dsize) {
	if (dsize == 0) {
		return dst;
	}

	do {
		*dst++ = *src;
	} while (--dsize != 0 && *src++ != 0);

	// If no null-term, don't need to backtrack
	if (dsize == 0 && *(dst - 1) != '\0') {
		return dst;
	}

	uchar *restrict trav = dst;
	while (dsize-- != 0) {
		*trav++ = 0;
	}

	// Back track to null-term byte
	return dst - 1;
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
