/**
 * @file vector-ext.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.0
 * @date 2024
 * @license LGPLv3.0
 * @brief Vector extension functions.
 */
#include "vector-ext.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <c-utils/vector.h>

// Growth factor
#define FACTOR 2

// Pointer arithmetic for elements
#define ptr_at(vec, index) (vec->data + vec->_type_size * index)

vector_status vec_bulk_push(vector *vec, const void *items, uint32_t count) {
	if (vec == NULL) {
		return VECTOR_STATUS_NULL;
	}

	// Exponentially grow until new elements can fit
	uint32_t old_alloc = vec->_alloc_count;
	while (vec->count < vec->_alloc_count + count) {
		// clang-format off
		vec->_alloc_count = (vec->data == NULL)
			? FACTOR
			: vec->_alloc_count * FACTOR;
		// clang-format on
	}
	if (vec->_alloc_count > old_alloc) {
		vec_reserve(vec, vec->_alloc_count);
	}

	// Copy elements
	void *push_ptr = ptr_at(vec, vec->count);
	memcpy(push_ptr, items, vec->_type_size * count);
	vec->count += count;

	return VECTOR_STATUS_OK;
}

vector_status vec_bulk_insert(vector *vec, uint32_t index, const void *value, uint32_t count) {
	if (vec == NULL) {
		return VECTOR_STATUS_NULL;
	}

	if (index >= vec->count) {
		return VECTOR_STATUS_BOUNDS;
	}

	// Exponentially grow until new elements can fit
	uint32_t old_alloc = vec->_alloc_count;
	while (vec->count < vec->_alloc_count + count) {
		// clang-format off
		vec->_alloc_count = (vec->data == NULL)
			? FACTOR
			: vec->_alloc_count * FACTOR;
		// clang-format on
	}
	if (vec->_alloc_count > old_alloc) {
		vec_reserve(vec, vec->_alloc_count);
	}

	// Move elements forwards
	uint32_t move_to = vec->count + count - 1;
	uint32_t last_insert_index = index + count - 1;
	while (move_to > last_insert_index) {
		uint32_t remaining = move_to - last_insert_index;
		uint32_t copy_count = (remaining >= count) ? count : remaining;

		memcpy(ptr_at(vec, move_to), ptr_at(vec, move_to - count), vec->_type_size * copy_count);
		move_to -= copy_count;
	}

	// Place element
	memcpy(ptr_at(vec, index), value, vec->_type_size * count);
	vec->count += count;

	return VECTOR_STATUS_OK;
}

vector_status vec_bulk_erase(vector *vec, uint32_t index, void *buffer, uint32_t count) {
	if (vec == NULL) {
		return VECTOR_STATUS_NULL;
	}

	if (index > vec->count - count) {
		return VECTOR_STATUS_BOUNDS;
	}

	// Copy elements if needed
	if (buffer != NULL) {
		memcpy(buffer, ptr_at(vec, index), vec->_type_size * count);
	}

	// Move elements backwards
	uint32_t move_from = index + count;
	while (move_from < vec->count) {
		uint32_t remaining = vec->count - move_from;
		uint32_t copy_count = (remaining >= count) ? count : remaining;

		memcpy(ptr_at(vec, move_from - count), ptr_at(vec, move_from), vec->_type_size * copy_count);
		move_from += copy_count;
	}

	vec->count -= count;

	return VECTOR_STATUS_OK;
}
