extern "C" {
#include "../include/vector.h"
}

#include <cstdlib>
#include <gtest/gtest.h>

static char element0 = '0';
static char element1 = '1';
static char element2 = '2';

static int int_element0 = 123;
static int int_element1 = 456;
static int int_element2 = 789;

TEST(Vector, VecInitOk) {
	vector vec = vec_init(sizeof(char));

	EXPECT_EQ(vec.data, nullptr);
	EXPECT_EQ(vec.count, 0);
}

TEST(Vector, VecNewOk) {
	vector *vec = vec_new(sizeof(char));

	EXPECT_NE(vec, nullptr);
	EXPECT_EQ(vec->data, nullptr);
	EXPECT_EQ(vec->count, 0);

	free(vec);
}

TEST(Vector, VecDeinitNullArg) {
	vector *vec = nullptr;

	EXPECT_EQ(vec_deinit(vec), VECTOR_STATUS_NULL);
}

TEST(Vector, VecDeinitNullData) {
	vector vec = {
		.data = nullptr,
		.count = 0,
		._type_size = 1,
		._alloc_count = 0,
	};

	EXPECT_EQ(vec_deinit(&vec), VECTOR_STATUS_OK);
}

TEST(Vector, VecDeinitOk) {
	vector vec = {
		.data = malloc(8),
		.count = 0,
		._type_size = 1,
		._alloc_count = 8,
	};

	EXPECT_EQ(vec_deinit(&vec), VECTOR_STATUS_OK);
}

TEST(Vector, VecDeleteNullArg) {
	vector *vec = nullptr;

	EXPECT_EQ(vec_delete(vec), VECTOR_STATUS_NULL);
}

TEST(Vector, VecDeleteNullData) {
	vector *vec = (vector *)malloc(sizeof(vector));
	vec->data = nullptr;

	EXPECT_EQ(vec_delete(vec), VECTOR_STATUS_OK);
}

TEST(Vector, VecDeleteOk) {
	vector *vec = (vector *)malloc(sizeof(vector));
	vec->data = malloc(8);

	EXPECT_EQ(vec_delete(vec), VECTOR_STATUS_OK);
}

TEST(Vector, VecReserveNull) {
	vector *vec = nullptr;

	EXPECT_EQ(vec_reserve(vec, 10), VECTOR_STATUS_NULL);
}

TEST(Vector, VecReserveEmpty) {
	vector vec = {
		.data = nullptr,
		.count = 0,
		._type_size = 1,
		._alloc_count = 0,
	};

	EXPECT_EQ(vec_reserve(&vec, 6), VECTOR_STATUS_OK);
	EXPECT_NE(vec.data, nullptr);
	EXPECT_GE(vec._alloc_count, 6);

	free(vec.data);
}

TEST(Vector, VecReserveOk) {
	vector vec = {
		.data = malloc(8),
		.count = 0,
		._type_size = 1,
		._alloc_count = 8,
	};

	// Less than allocated
	EXPECT_EQ(vec_reserve(&vec, 6), VECTOR_STATUS_OK);
	EXPECT_GE(vec._alloc_count, 6);

	// Same as allocated
	EXPECT_EQ(vec_reserve(&vec, 8), VECTOR_STATUS_OK);
	EXPECT_GE(vec._alloc_count, 8);

	// More than allocated
	EXPECT_EQ(vec_reserve(&vec, 10), VECTOR_STATUS_OK);
	EXPECT_GE(vec._alloc_count, 10);

	free(vec.data);
}

TEST(Vector, VecPushNull) {
	vector *vec = nullptr;

	EXPECT_EQ(vec_push(vec, nullptr), VECTOR_STATUS_NULL);
}

TEST(Vector, VecPushOk) {
	vector vec = {
		.data = nullptr,
		.count = 0,
		._type_size = sizeof(char),
		._alloc_count = 0,
	};

	EXPECT_EQ(vec_push(&vec, &element0), VECTOR_STATUS_OK);
	EXPECT_NE(vec.data, nullptr);
	EXPECT_EQ(*((char *)vec.data), element0);
	EXPECT_EQ(vec.count, 1);

	EXPECT_EQ(vec_push(&vec, &element1), VECTOR_STATUS_OK);
	EXPECT_EQ(*((char *)vec.data + 1), element1);
	EXPECT_EQ(vec.count, 2);

	EXPECT_EQ(vec_push(&vec, &element2), VECTOR_STATUS_OK);
	EXPECT_EQ(*((char *)vec.data + 2), element2);
	EXPECT_EQ(vec.count, 3);

	// Verify that previous elements are still there
	EXPECT_EQ(*((char *)vec.data), element0);
	EXPECT_EQ(*((char *)vec.data + 1), element1);

	free(vec.data);
}

TEST(Vector, VecInsertNull) {
	vector *vec = nullptr;

	EXPECT_EQ(vec_insert(vec, 0, nullptr), VECTOR_STATUS_NULL);
}

TEST(Vector, VecInsertBounds) {
	vector vec = {
		.data = nullptr,
		.count = 0,
		._type_size = sizeof(char),
		._alloc_count = 0,
	};

	EXPECT_EQ(vec_insert(&vec, 1, &element0), VECTOR_STATUS_BOUNDS);
	EXPECT_EQ(vec.data, nullptr);
	EXPECT_EQ(vec.count, 0);
}

TEST(Vector, VecInsertOk) {
	vector vec = {
		.data = nullptr,
		.count = 0,
		._type_size = sizeof(char),
		._alloc_count = 0,
	};

	EXPECT_EQ(vec_insert(&vec, 0, &element0), VECTOR_STATUS_OK);
	EXPECT_NE(vec.data, nullptr);
	EXPECT_EQ(*((char *)vec.data), element0);
	EXPECT_EQ(vec.count, 1);

	EXPECT_EQ(vec_insert(&vec, 1, &element1), VECTOR_STATUS_OK);
	EXPECT_EQ(*((char *)vec.data + 1), element1);
	EXPECT_EQ(vec.count, 2);

	EXPECT_EQ(vec_insert(&vec, 0, &element2), VECTOR_STATUS_OK);
	EXPECT_EQ(*((char *)vec.data), element2);
	EXPECT_EQ(vec.count, 3);

	// Verify other elements
	EXPECT_EQ(*((char *)vec.data + 1), element0);
	EXPECT_EQ(*((char *)vec.data + 2), element1);

	EXPECT_EQ(vec_insert(&vec, 1, &element1), VECTOR_STATUS_OK);
	EXPECT_EQ(*((char *)vec.data + 1), element1);
	EXPECT_EQ(vec.count, 4);

	// Verify other elements
	EXPECT_EQ(*((char *)vec.data), element2);
	EXPECT_EQ(*((char *)vec.data + 1), element1);
	EXPECT_EQ(*((char *)vec.data + 2), element0);
	EXPECT_EQ(*((char *)vec.data + 3), element1);

	free(vec.data);
}

TEST(Vector, VecInsertOkMultibyte) {
	vector vec = {
		.data = nullptr,
		.count = 0,
		._type_size = sizeof(int),
		._alloc_count = 0,
	};

	EXPECT_EQ(vec_insert(&vec, 0, &int_element0), VECTOR_STATUS_OK);
	EXPECT_NE(vec.data, nullptr);
	EXPECT_EQ(*((int *)vec.data), int_element0);
	EXPECT_EQ(vec.count, 1);

	EXPECT_EQ(vec_insert(&vec, 1, &int_element1), VECTOR_STATUS_OK);
	EXPECT_EQ(*((int *)vec.data + 1), int_element1);
	EXPECT_EQ(vec.count, 2);

	EXPECT_EQ(vec_insert(&vec, 0, &int_element2), VECTOR_STATUS_OK);
	EXPECT_EQ(*((int *)vec.data), int_element2);
	EXPECT_EQ(vec.count, 3);

	// Verify other elements
	EXPECT_EQ(*((int *)vec.data + 1), int_element0);
	EXPECT_EQ(*((int *)vec.data + 2), int_element1);

	EXPECT_EQ(vec_insert(&vec, 1, &int_element1), VECTOR_STATUS_OK);
	EXPECT_EQ(*((int *)vec.data + 1), int_element1);
	EXPECT_EQ(vec.count, 4);

	// Verify other elements
	EXPECT_EQ(*((int *)vec.data), int_element2);
	EXPECT_EQ(*((int *)vec.data + 1), int_element1);
	EXPECT_EQ(*((int *)vec.data + 2), int_element0);
	EXPECT_EQ(*((int *)vec.data + 3), int_element1);

	free(vec.data);
}

TEST(Vector, VecEraseNull) {
	vector *vec = nullptr;

	EXPECT_EQ(vec_erase(vec, 0, nullptr), VECTOR_STATUS_NULL);
}

TEST(Vector, VecEraseBounds) {
	vector vec = {
		.data = malloc(sizeof(char) * 2),
		.count = 2,
		._type_size = sizeof(char),
		._alloc_count = 2,
	};

	*((char *)vec.data) = element0;
	*((char *)vec.data + 1) = element1;

	EXPECT_EQ(vec_erase(&vec, 2, nullptr), VECTOR_STATUS_BOUNDS);
	EXPECT_EQ(vec.count, 2);

	EXPECT_EQ(vec_erase(&vec, 1, nullptr), VECTOR_STATUS_OK);
	EXPECT_EQ(vec.count, 1);

	EXPECT_EQ(vec_erase(&vec, 1, nullptr), VECTOR_STATUS_BOUNDS);
	EXPECT_EQ(vec.count, 1);

	free(vec.data);
}

TEST(Vector, VecEraseOk) {
	vector vec = {
		.data = malloc(sizeof(char) * 3),
		.count = 3,
		._type_size = sizeof(char),
		._alloc_count = 3,
	};

	*((char *)vec.data) = element0;
	*((char *)vec.data + 1) = element1;
	*((char *)vec.data + 2) = element2;

	char buffer = 123;
	EXPECT_EQ(vec_erase(&vec, 0, &buffer), VECTOR_STATUS_OK);
	EXPECT_EQ(buffer, element0);
	EXPECT_EQ(vec.count, 2);

	// Verify elements
	EXPECT_EQ(*((char *)vec.data), element1);
	EXPECT_EQ(*((char *)vec.data + 1), element2);

	EXPECT_EQ(vec_erase(&vec, 1, &buffer), VECTOR_STATUS_OK);
	EXPECT_EQ(buffer, element2);
	EXPECT_EQ(vec.count, 1);

	// Verify elements
	EXPECT_EQ(*((char *)vec.data), element1);

	free(vec.data);
}

TEST(Vector, VecAtNull) {
	vector *vec = nullptr;

	EXPECT_EQ(vec_at(vec, 0), nullptr);
}

TEST(Vector, VecAtBounds) {
	vector vec = {
		.data = malloc(sizeof(char) * 2),
		.count = 2,
		._type_size = sizeof(char),
		._alloc_count = 2,
	};

	EXPECT_EQ(vec_at(&vec, 3), nullptr);
	EXPECT_EQ(vec_at(&vec, 2), nullptr);
}

TEST(Vector, VecAtOk) {
	vector vec = {
		.data = malloc(sizeof(char) * 2),
		.count = 2,
		._type_size = sizeof(char),
		._alloc_count = 2,
	};

	*((char *)vec.data) = element0;
	*((char *)vec.data + 1) = element1;

	EXPECT_EQ(*(char *)vec_at(&vec, 0), element0);
	EXPECT_EQ(*(char *)vec_at(&vec, 1), element1);
}

TEST(Vector, VecCollectNull) {
	vector *vec = nullptr;

	EXPECT_EQ(vec_collect(vec), nullptr);
}

TEST(Vector, VecCollectOk) {
	void *memory = malloc(8);

	vector vec = {
		.data = memory,
		.count = 0,
		._type_size = 1,
		._alloc_count = 8,
	};

	EXPECT_EQ(vec_collect(&vec), memory);
	EXPECT_EQ(vec.data, nullptr);
	EXPECT_EQ(vec._alloc_count, 0);

	free(memory);
}

TEST(Vector, FullTest) {
	vector *vec = vec_new(sizeof(char));

	EXPECT_EQ(vec_push(vec, &element0), VECTOR_STATUS_OK);
	EXPECT_EQ(vec_push(vec, &element1), VECTOR_STATUS_OK);
	EXPECT_EQ(vec_push(vec, &element2), VECTOR_STATUS_OK);

	EXPECT_EQ(*(char *)vec_at(vec, 0), element0);
	EXPECT_EQ(*(char *)vec_at(vec, 1), element1);
	EXPECT_EQ(*(char *)vec_at(vec, 2), element2);

	char buffer = 123;

	EXPECT_EQ(vec_erase(vec, 1, &buffer), VECTOR_STATUS_OK);
	EXPECT_EQ(buffer, element1);
	EXPECT_EQ(vec_insert(vec, 1, &buffer), VECTOR_STATUS_OK);

	EXPECT_EQ(*(char *)vec_at(vec, 0), element0);
	EXPECT_EQ(*(char *)vec_at(vec, 1), element1);
	EXPECT_EQ(*(char *)vec_at(vec, 2), element2);

	char *inner_data = (char *)vec_collect(vec);
	EXPECT_NE(inner_data, nullptr);

	EXPECT_EQ(vec_delete(vec), VECTOR_STATUS_OK);

	EXPECT_EQ(inner_data[0], element0);
	EXPECT_EQ(inner_data[1], element1);
	EXPECT_EQ(inner_data[2], element2);

	free(inner_data);
}
