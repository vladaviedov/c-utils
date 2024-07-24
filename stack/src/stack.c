/**
 * @file stack.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.0.1
 * @date 2024
 * @license LGPLv3.0
 * @brief Abstract stack.
 */
#include "stack.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Growth factor
#define FACTOR 2

// Pointer arithmetic for elements
#define ptr_at(st, index) (st->_data + st->_type_size * (index))

stack stack_init(size_t type_size) {
	stack st = {
		._data = NULL,
		._type_size = type_size,
		.count = 0,
		._alloc_count = 0,
	};

	return st;
}

stack *stack_new(size_t type_size) {
	stack *st = malloc(sizeof(stack));

	st->_data = NULL;
	st->_type_size = type_size;
	st->count = 0;
	st->_alloc_count = 0;

	return st;
}

stack_status stack_deinit(stack *st) {
	if (st == NULL) {
		return STACK_STATUS_NULL;
	}

	if (st->_data != NULL) {
		free(st->_data);
	}

	return STACK_STATUS_OK;
}

stack_status stack_delete(stack *st) {
	if (stack_deinit(st) == STACK_STATUS_NULL) {
		return STACK_STATUS_NULL;
	}

	free(st);
	return STACK_STATUS_OK;
}

stack_status stack_reserve(stack *st, uint32_t count) {
	if (st == NULL) {
		return STACK_STATUS_NULL;
	}

	if (count > st->_alloc_count) {
		st->_data = realloc(st->_data, st->_type_size * count);
		st->_alloc_count = count;
	}

	return STACK_STATUS_OK;
}

stack_status stack_push(stack *st, const void *value) {
	if (st == NULL) {
		return STACK_STATUS_NULL;
	}

	// New to grow array
	if (st->count == st->_alloc_count) {
		// clang-format off
		uint32_t new_count = (st->_data == NULL)
			? FACTOR
			: st->_alloc_count * FACTOR;
		// clang-format on
		stack_reserve(st, new_count);
	}

	// Copy element
	void *push_ptr = ptr_at(st, st->count);
	memcpy(push_ptr, value, st->_type_size);

	st->count++;
	return STACK_STATUS_OK;
}

stack_status stack_pop(stack *st, void *buffer) {
	if (st == NULL) {
		return STACK_STATUS_NULL;
	}

	if (st->count == 0) {
		return STACK_STATUS_EMPTY;
	}

	// Copy element if needed
	if (buffer != NULL) {
		memcpy(buffer, ptr_at(st, st->count - 1), st->_type_size);
	}

	st->count--;
	return STACK_STATUS_OK;
}

const void *stack_peek(const stack *st) {
	if (st == NULL || st->count == 0) {
		return NULL;
	}

	return ptr_at(st, st->count - 1);
}
