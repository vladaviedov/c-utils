/**
 * @file ustring.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.0
 * @date 2025
 * @license LGPLv3.0
 * @brief Unicode string library.
 */
#pragma once

#include <stddef.h>

#include "uchar.h"

/**
 * @brief Copy the string from 'src' to the buffer 'dst'.
 *
 * @param[out] dst - Destination buffer.
 * @param[in] src - Original string.
 * @return Destination buffer.
 * @note 'src' and 'dst' may not overlap
 */
uchar *ustrcpy(uchar *restrict dst, const uchar *restrict src);

/**
 * @brief Copy at most 'dsize' points from 'src' to the buffer 'dst', padding
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
 * @brief Create a duplicate of 's', allocating memory in the process.
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
 * @brief Concatenate the string 'src' into 'dst'.
 *
 * @param[in,out] dst - First string and destination buffer.
 * @param[in] src - Second string.
 * @return Destination buffer.
 */
uchar *ustrcat(uchar *restrict dst, const uchar *restrict src);

/**
 * @brief Concatenate at most 'ssize' points from 'src' to 'dst',
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
 * @brief Compare two strings.
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

/**
 * @brief Copy the string from 'src' to the buffer 'dst'.
 *
 * @param[out] dst - Destination buffer.
 * @param[in] src - Original string.
 * @return Pointer to the null-terminator of 'dst'.
 * @note 'src' and 'dst' may not overlap
 */
uchar *ustpcpy(uchar *restrict dst, const uchar *restrict src);

/**
 * @brief Copy at most 'dsize' points from 'src' to the buffer 'dst', padding
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

/**
 * @brief Find the first occurrence of a point inside the string.
 *
 * @param[in] s - String to search.
 * @param[in] c - Point to find.
 * @return NULL - Point not found.\n
 *         Value - Pointer to the first occurrence inside 's'.
 */
uchar *ustrchr(const uchar *s, uchar c);

/**
 * @brief Find the last occurrence of a point inside the string.
 *
 * @param[in] s - String to search.
 * @param[in] c - Point to find.
 * @return NULL - Point not found.\n
 *         Value - Pointer to the last occurrence inside 's'.
 */
uchar *ustrrchr(const uchar *s, uchar c);

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

/**
 * @brief Find the first occurrence of a value in the set inside a string.
 *
 * @param[in] s - String to search.
 * @param[in] accept - Set of points to search for.
 * @return NULL - Point not found.\n
 *         Value - Pointer to the first occurrence inside 's'.
 */
uchar *ustrpbrk(const uchar *s, const uchar *accept);

// TODO: implement in later version
// These functions require a good string searching algorithm.
//
// size_t ustrspn(const char *s, const char *accept);
// size_t ustrcspn(const uchar *s, const uchar *reject);
// uchar *ustrstr(const uchar *haystack, const uchar *needle);
// uchar *ustrtok(uchar *restrict str, const uchar *restrict delim);
// uchar *ustrtok_r(
// 	uchar *restrict str, const uchar *restrict delim, uchar **restrict saveptr);

// TODO: implement in later version
// These functions require the UCA and/or locale stuff to be implemented to work
// correctly.
//
// int ustrcoll(const uchar *s1, const uchar *s2);
// size_t ustrxfrm(char *restrict, const char *restrict, size_t);
// int ustrcoll_l(const uchar *s1, const uchar *s2);
// char *ustrerror(int);
// char *ustrerror_l(int, locale_t a);
// int ustrerror_r(int, char *, size_t);
// char *ustrsignal(int);
