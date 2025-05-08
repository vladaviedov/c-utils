/**
 * @file ucwidth.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version dev
 * @date 2025
 * @license LGPLv3.0
 * @brief Unicode character width.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "uchar.h"

/**
 * @brief Get the number of columns needed to represent a single unicode point.
 *
 * @param[in] uc - Unicode point.
 * @return >1 - Count of columns needed.\n
 *         0 - Input is a null character.\n
 *         -1 - Input is a ASCII control character.
 */
int ucwidth(uchar uc);

/**
 * @brief Get the number of columns needed to represent a unicode point string,
 * with a maximum value of 'n'.
 *
 * @param[in] s - Unicode string.
 * @param[in] n - Maximum length.
 * @return 0~n - Count of columns needed.\n
 *         -1 - String contains ASCII control characters.
 */
int ucswidth(const uchar *s, size_t n);
