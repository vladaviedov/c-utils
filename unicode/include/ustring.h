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

/**
 * @brief Copies the string from 'src' to the buffer 'dst'.
 *
 * @param[out] dst - Destination buffer.
 * @param[in] src - Original string.
 * @return Destination buffer.
 * @note 'src' and 'dst' may not overlap
 */
uchar *ustrcpy(uchar *restrict dst, const uchar *restrict src);

/**
 * @brief Copies at most 'dsize' points from 'src' to the buffer 'dst', padding
 * 'dst' with zeros if the size of 'src' is smaller than 'dsize'.
 *
 * @param[out] dst - Destination buffer.
 * @param[in] src - Original string.
 * @param[in] dsize - Size of 'dst'.
 * @return Destination buffer.
 * @note 'src' and 'dst' may not overlap
 * @warning This function does not ensure that 'dst' is a valid ustring.
 */
uchar *ustrncpy(uchar *restrict dst, const uchar *restrict src, size_t dsize);

/**
 * @brief Creates a duplicate of 's', allocating memory in the process.
 *
 * @param[in] s - Original string.
 * @return Duplicate string or NULL on error.
 * @note Memory must be freed by the caller.
 */
uchar *ustrdup(const uchar *s);

/**
 * @brief Duplicate at most 'n' points from 's', allocating memory in the
 * process. A null-terminator is added at the end of the string.
 *
 * @param[in] s - Original string.
 * @param[in] n - Maximum amount of points to copy.
 * @return Duplicate string or NULL on error.
 * @note Memory must be freed by the caller.
 */
uchar *ustrndup(const uchar *s, size_t n);

/**
 * @brief Concatenates the string 'src' into 'dst'.
 *
 * @param[in,out] dst - First string and destination buffer.
 * @param[in] src - Second string.
 * @return Destination buffer.
 */
uchar *ustrcat(uchar *restrict dst, const uchar *restrict src);

/**
 * @brief Concatenates at most 'ssize' points from 'src' to 'dst',
 * adding a null-terminator at the end.
 *
 * @param[in,out] dst - First string and destination buffer.
 * @param[in] src - Second string.
 * @param[in] ssize - Count of points to copy from 'src'.
 * @return Destination buffer.
 * @note 'src' and 'dst' may not overlap
 */
uchar *ustrncat(uchar *restrict dst, const uchar *restrict src, size_t ssize);

/**
 * @brief Compares the two strings.
 *
 * @param[in] s1 - First string.
 * @param[in] s2 - Second string.
 * @return 0 - s1 and s2 are equal.\n
 *         <0 - s1 is less than s2.\n
 *         >0 - s1 is greather than s2.
 */
int ustrcmp(const uchar *s1, const uchar *s2);

/**
 * @brief Compare at most 'ssize' points of the two strings.
 *
 * @param[in] s1 - First string.
 * @param[in] s2 - Second string.
 * @param[in] ssize - Maximum count of points to compare.
 * @return 0 - 'ssize' points of s1 and s2 are equal.\n
 *         <0 - 'ssize' points of s1 is less than s2.\n
 *         >0 - 'ssize' points of s1 is greather than s2.
 */
int ustrncmp(const uchar *s1, const uchar *s2, size_t ssize);

int ustrcoll(const uchar *s1, const uchar *s2);

size_t ustrxfrm(char *restrict, const char *restrict, size_t);

/**
 * @brief Copies the string from 'src' to the buffer 'dst'.
 *
 * @param[out] dst - Destination buffer.
 * @param[in] src - Original string.
 * @return Pointer to the null-terminator of 'dst'.
 * @note 'src' and 'dst' may not overlap
 */
uchar *ustpcpy(uchar *restrict dst, const uchar *restrict src);

/**
 * @brief Copies at most 'dsize' points from 'src' to the buffer 'dst', padding
 * with zeros if the size of 'src' is smaller than 'dsize'.
 *
 * @param[out] dst - Destination buffer.
 * @param[in] src - Original string.
 * @param[in] dsize - Size of 'dst'.
 * @return Pointer to the address one after the last non-zero uchar copied.
 * @note 'src' and 'dst' may not overlap
 * @warning This function does not ensure that 'dst' is a valid ustring.
 */
uchar *ustpncpy(uchar *restrict dst, const uchar *restrict src, size_t dsize);

uchar *ustrchr(const uchar *s, uchar c);
/* int ustrcoll_l(const uchar *s1, const uchar *s2); */
size_t ustrcspn(const uchar *s, const uchar *reject);
char *ustrerror(int);
/* char *ustrerror_l(int, locale_t a); */
int ustrerror_r(int, char *, size_t);

/**
 * @brief Calculate the length of 's'.
 *
 * @param[in] s - String.
 * @return Count of non-zero points in 's'.
 */
size_t ustrlen(const uchar *s);

/**
 * @brief Calculate the minumum of the length of 's' and 'maxlen'. Does not
 * traverse the string past 'maxlen'.
 *
 * @param[in] s - String.
 * @param[in] maxlen - Maximum amount of points to inspect.
 * @return Count of non-zero points in 's' or 'maxlen'.
 */
size_t ustrnlen(const uchar *s, size_t maxlen);

char *ustrpbrk(const char *, const char *);
char *ustrrchr(const char *, int);
char *ustrsignal(int);
size_t ustrspn(const char *, const char *);
char *ustrstr(const char *, const char *);
char *ustrtok(char *restrict, const char *restrict);
char *ustrtok_r(char *restrict, const char *restrict, char **restrict);
