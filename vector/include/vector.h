/**
 * @file vector.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.1
 * @date 2024
 * @license LGPLv3.0
 * @brief Abstract vector.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @struct vector
 * Vector object. Fields should not be edited.
 *
 * @var vector::data
 * Raw data array.
 * @var vector::count
 * Current element count.
 *
 * @internal
 *
 * @var vector::_type_size
 * Size of contained type.
 * @var vector::_alloc_count
 * How many elements can fit in data.
 *
 * @endinternal
 */
typedef struct {
	void *data;
	uint32_t count;

	size_t _type_size;
	uint32_t _alloc_count;
} vector;

/**
 * @enum vector_status
 * Result of vector operation.
 *
 * @var vector_status::VECTOR_STATUS_OK
 * Operation completed successfully.
 *
 * @var vector_status::VECTOR_STATUS_NULL
 * Vector argument is null.
 *
 * @var vector_status::VECTOR_STATUS_BOUNDS
 * Operation was out of bounds.
 */
typedef enum {
	VECTOR_STATUS_OK = 0,
	VECTOR_STATUS_NULL = 1,
	VECTOR_STATUS_BOUNDS = 2,
} vector_status;

/**
 * @brief Create vector object on the stack.
 *
 * @param[in] type_size - sizeof result of the desired type.
 * @return Vector object.
 * @note Delete with vec_deinit.
 */
vector vec_init(size_t type_size);

/**
 * @brief Create vector object on the heap.
 *
 * @param[in] type_size - sizeof result of the desired type.
 * @return Vector object.
 * @note Delete with vec_delete.
 */
vector *vec_new(size_t type_size);

/**
 * @brief Delete vector object from the stack.
 *
 * @param[in] vec - Vector object.
 * @return Status code.
 */
vector_status vec_deinit(vector *vec);

/**
 * @brief Delete vector object from the heap.
 *
 * @param[in] vec - Vector object.
 * @return Status code.
 */
vector_status vec_delete(vector *vec);

/**
 * @brief Reserve space for elements.
 *
 * @param[in,out] vec - Vector object.
 * @param[in] count - Amount of elements to reserve.
 * @return Status code.
 * @note Will only grow the object.
 */
vector_status vec_reserve(vector *vec, uint32_t count);

/**
 * @brief Add element to the end.
 *
 * @param[in,out] vec - Vector object.
 * @param[in] value - New element.
 * @return Status code.
 */
vector_status vec_push(vector *vec, const void *value);

/**
 * @brief Add element at specified location.
 *
 * @param[in,out] vec - Vector object.
 * @param[in] index - Insert index.
 * @param[in] value - New element.
 * @return Status code.
 */
vector_status vec_insert(vector *vec, uint32_t index, const void *value);

/**
 * @brief Remove element at specified location.
 *
 * @param[in,out] vec - Vector object.
 * @param[in] index - Removal index.
 * @param[out] buffer - If not NULL, erased value placed here.
 * @return Status code.
 */
vector_status vec_erase(vector *vec, uint32_t index, void *buffer);

/**
 * @brief Access element at specified location (const).
 *
 * @param[in] vec - Vector object.
 * @param[in] index - Access index.
 * @return Pointer to element (const).
 */
const void *vec_at(const vector *vec, uint32_t index);

/**
 * @brief Access element at specified location (mutable).
 *
 * @param[in] vec - Vector object.
 * @param[in] index - Access index.
 * @return Pointer to element (mutable).
 */
void *vec_at_mut(const vector *vec, uint32_t index);

/**
 * @brief Collect vector data array, resetting the vector.
 *
 * @param[in] vec - Vector object.
 * @return Data array.
 * @note User is responsible for freeing the memory.
 */
void *vec_collect(vector *vec);
