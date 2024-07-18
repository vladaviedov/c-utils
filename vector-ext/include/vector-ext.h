/**
 * @file vector-ext.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.0
 * @date 2024
 * @license LGPLv3.0
 * @brief Vector extension functions.
 */
#pragma once

#include <c-utils/vector.h>

/**
 * @brief Add multiple elements to the end.
 *
 * @param[in,out] vec - Vector object.
 * @param[in] value - Array of new elements.
 * @param[in] count - Count of elements to push.
 * @return Status code.
 */
vector_status vec_bulk_push(vector *vec, const void *items, uint32_t count);

/**
 * @brief Add multiple elements at specified location.
 *
 * @param[in,out] vec - Vector object.
 * @param[in] index - Insertion start index.
 * @param[in] value - Array of new elements.
 * @param[in] count - Count of elements to insert.
 * @return Status code.
 */
vector_status vec_bulk_insert(vector *vec, uint32_t index, const void *items, uint32_t count);

/**
 * @brief Remove multiple elements at specified location.
 *
 * @param[in,out] vec - Vector object.
 * @param[in] index - Removal index.
 * @param[out] buffer - If not NULL, erased values placed here (as array).
 * @param[in] count - Count of elements to remove.
 * @return Status code.
 */
vector_status vec_bulk_erase(vector *vec, uint32_t index, void *buffer, uint32_t count);
