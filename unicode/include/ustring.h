/**
 * @file ustring.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version dev
 * @date 2025
 * @license LGPLv3.0
 * @brief string.h equivalent for unicode point strings.
 */
#pragma once

#include <stddef.h>

#include "uchar.h"

uchar *ustrcpy(uchar *restrict dst, const uchar *restrict src);
uchar *ustrncpy(uchar *restrict dst, const uchar *restrict src, size_t dsize);
uchar *ustrdup(const uchar *s);
uchar *ustrndup(const uchar *s, size_t n);

uchar *ustrcat(uchar *restrict dst, const uchar *restrict src);
uchar *ustrncat(uchar *restrict dst, const uchar *restrict src, size_t ssize);
int ustrcmp(const uchar *s1, const uchar *s2);
int ustrcoll(const uchar *s1, const uchar *s2);
uchar *ustrncmp(const uchar *s1, const uchar *s2, size_t ssize);

size_t ustrxfrm(char *restrict, const char *restrict, size_t);

uchar *ustpcpy(uchar *restrict dst, const uchar *restrict src);
uchar *ustpncpy(uchar *restrict dst, const uchar *restrict src, size_t dsize);
uchar *ustrchr(const uchar *s, uchar c);
/* int ustrcoll_l(const uchar *s1, const uchar *s2); */
size_t ustrcspn(const uchar *s, const uchar *reject);
char *ustrerror(int);
/* char *ustrerror_l(int, locale_t a); */
int ustrerror_r(int, char *, size_t);
size_t ustrlen(const uchar *ustr);
size_t ustrnlen(const uchar *ustr, size_t maxlen);
char *ustrpbrk(const char *, const char *);
char *ustrrchr(const char *, int);
char *ustrsignal(int);
size_t ustrspn(const char *, const char *);
char *ustrstr(const char *, const char *);
char *ustrtok(char *restrict, const char *restrict);
char *ustrtok_r(char *restrict, const char *restrict, char **restrict);
