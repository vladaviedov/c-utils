/**
 * @file vector.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.0
 * @date 2024
 * @license LGPLv3.0
 * @brief Abstract vector.
 */
#include <vector.h>

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Growth factor
#define FACTOR 2

// Pointer arithmetic for elements
#define ptr_at(vec, index) (vec->data + vec->_type_size * index)

vector vec_init(size_t type_size) {
	vector vec = {
		.data = NULL,
		._type_size = type_size,
		.count = 0,
		._alloc_count = 0,
	};

	return vec;
}

vector *vec_new(size_t type_size) {
	vector *vec = malloc(sizeof(vector));

	vec->data = NULL;
	vec->_type_size = type_size;
	vec->count = 0;
	vec->_alloc_count = 0;

	return vec;
}

vector_status vec_deinit(vector *vec) {
	if (vec == NULL) {
		return VECTOR_STATUS_NULL;
	}

	if (vec->data != NULL) {
		free(vec->data);
	}

	return VECTOR_STATUS_OK;
}

vector_status vec_delete(vector *vec) {
	if (vec_deinit(vec) == VECTOR_STATUS_NULL) {
		return VECTOR_STATUS_NULL;
	}

	free(vec);
	return VECTOR_STATUS_OK;
}

vector_status vec_reserve(vector *vec, uint32_t count) {
	if (vec == NULL) {
		return VECTOR_STATUS_NULL;
	}

	if (count > vec->_alloc_count) {
		vec->data = realloc(vec->data, vec->_type_size * count);
		vec->_alloc_count = count;
	}

	return VECTOR_STATUS_OK;
}

vector_status vec_push(vector *vec, const void *value) {
	if (vec == NULL) {
		return VECTOR_STATUS_NULL;
	}

	// New to grow vector
	if (vec->count == vec->_alloc_count) {
		uint32_t new_count = (vec->data == NULL)
			? FACTOR
			: vec->_alloc_count * FACTOR;
		vec_reserve(vec, new_count);
	}

	// Copy element
	void *push_ptr = ptr_at(vec, vec->count);
	memcpy(push_ptr, value, vec->_type_size);

	vec->count++;
	return VECTOR_STATUS_OK;
}

vector_status vec_insert(vector *vec, uint32_t index, const void *value) {
	if (vec == NULL) {
		return VECTOR_STATUS_NULL;
	}

	// Up to last index + 1
	if (index > vec->count) {
		return VECTOR_STATUS_BOUNDS;
	}

	// New to grow vector
	if (vec->count == vec->_alloc_count) {
		uint32_t new_count = (vec->data == NULL)
			? FACTOR
			: vec->_alloc_count * FACTOR;
		vec_reserve(vec, new_count);
	}

	// Move elements forwards
	for (uint32_t i = vec->count; i > index; i--) {
		memcpy(ptr_at(vec, i), ptr_at(vec, i - 1), vec->_type_size);
	}

	// Place element
	memcpy(ptr_at(vec, index), value, vec->_type_size);
	vec->count++;

	return VECTOR_STATUS_OK;
}

vector_status vec_erase(vector *vec, uint32_t index, void *buffer) {
	if (vec == NULL) {
		return VECTOR_STATUS_NULL;
	}

	if (index >= vec->count) {
		return VECTOR_STATUS_BOUNDS;
	}

	// Copy element if needed
	if (buffer != NULL) {
		memcpy(buffer, ptr_at(vec, index), vec->_type_size);
	}

	// Move elements backwards
	for (uint32_t i = index + 1; i < vec->count; i++) {
		memcpy(ptr_at(vec, i - 1), ptr_at(vec, i), vec->_type_size);
	}
	vec->count--;

	return VECTOR_STATUS_OK;
}

const void *vec_at(const vector *vec, uint32_t index) {
	return vec_at_mut(vec, index);
}

void *vec_at_mut(const vector *vec, uint32_t index) {
	if (vec == NULL || index >= vec->count) {
		return NULL;
	}

	return ptr_at(vec, index);
}
