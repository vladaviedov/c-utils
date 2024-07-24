/**
 * @file stack.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.0.1
 * @date 2024
 * @license LGPLv3.0
 * @brief Abstract stack.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @struct stack
 * Stack object. Fields should not be edited.
 *
 * @var stack::count
 * Count of elements in the stack.
 *
 * @internal
 *
 * @var stack::_data
 * Raw data array.
 * @var stack::_type_size
 * Size of contained type.
 * @var stack::_alloc_count
 * How many elements can fit in data.
 *
 * @endinternal
 */
typedef struct {
	uint32_t count;

	void *_data;
	size_t _type_size;
	uint32_t _alloc_count;
} stack;

/**
 * @enum stack_status
 * Result of stack operation.
 *
 * @var stack_status::STACK_STATUS_OK
 * Operation completed successfully.
 *
 * @var stack_status::STACK_STATUS_NULL
 * Stack argument is null.
 *
 * @var stack_status::STACK_STATUS_EMPTY
 * Stack is empty.
 */
typedef enum {
	STACK_STATUS_OK = 0,
	STACK_STATUS_NULL = 1,
	STACK_STATUS_EMPTY = 2
} stack_status;

/**
 * @brief Create stack object on the stack.
 *
 * @param[in] type_size - sizeof result of the desired type.
 * @return Stack object.
 * @note Delete with stack_deinit.
 */
stack stack_init(size_t type_size);

/**
 * @brief Create stack object on the heap.
 *
 * @param[in] type_size - sizeof result of the desired type.
 * @return Stack object.
 * @note Delete with stack_delete.
 */
stack *stack_new(size_t type_size);

/**
 * @brief Delete stack object from the stack.
 *
 * @param[in] st - Stack object.
 * @return Status code.
 */
stack_status stack_deinit(stack *st);

/**
 * @brief Delete stack object from the heap.
 *
 * @param[in] st - Stack object.
 * @return Status code.
 */
stack_status stack_delete(stack *st);

/**
 * @brief Reserve space for elements.
 *
 * @param[in,out] st - Stack object.
 * @param[in] count - Amount of elements to reserve.
 * @return Status code.
 * @note Will only grow the object.
 */
stack_status stack_reserve(stack *st, uint32_t count);

/**
 * @brief Push element to the stack.
 *
 * @param[in,out] st - Stack object.
 * @param[in] value - Value to push.
 * @return Status code.
 */
stack_status stack_push(stack *st, const void *value);

/**
 * @brief Pop element from the stack.
 *
 * @param[in,out] st - Stack object.
 * @param[out] buffer - If not NULL, popped element is copied here.
 * @return Status code.
 */
stack_status stack_pop(stack *st, void *buffer);

/**
 * @brief Look at top element without altering the object.
 *
 * @param[in] st - Stack object.
 */
const void *stack_peek(const stack *st);
