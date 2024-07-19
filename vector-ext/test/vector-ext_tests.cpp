extern "C" {
#include <c-utils/vector.h>
#include "../include/vector-ext.h"
}

#include <cstdlib>
#include <gtest/gtest.h>

static char element0 = '0';
static char element1 = '1';
static char element2 = '2';

static char elements0[] = { element0, element1, element2 };
static char elements1[] = { element2, element0, element1 };

TEST(VectorExt, VecBulkPushNull) {
	vector *vec = nullptr;

	EXPECT_EQ(vec_bulk_push(vec, nullptr, 1), VECTOR_STATUS_NULL);
}

TEST(VectorExt, VecBulkPushOk) {
	vector vec = {
		.data = nullptr,
		.count = 0,
		._type_size = sizeof(char),
		._alloc_count = 0,
	};

	EXPECT_EQ(vec_bulk_push(&vec, elements0, 3), VECTOR_STATUS_OK);
	EXPECT_NE(vec.data, nullptr);
	EXPECT_EQ(*((char *)vec.data), element0);
	EXPECT_EQ(*((char *)vec.data + 1), element1);
	EXPECT_EQ(*((char *)vec.data + 2), element2);
	EXPECT_EQ(vec.count, 3);

	EXPECT_EQ(vec_bulk_push(&vec, elements1, 3), VECTOR_STATUS_OK);
	EXPECT_EQ(*((char *)vec.data + 3), element2);
	EXPECT_EQ(*((char *)vec.data + 4), element0);
	EXPECT_EQ(*((char *)vec.data + 5), element1);
	EXPECT_EQ(vec.count, 6);

	// Verify that previous elements are still there
	EXPECT_EQ(*((char *)vec.data), element0);
	EXPECT_EQ(*((char *)vec.data + 1), element1);
	EXPECT_EQ(*((char *)vec.data + 2), element2);

	free(vec.data);
}

TEST(VectorExt, VecBulkInsertNull) {
	vector *vec = nullptr;

	EXPECT_EQ(vec_bulk_insert(vec, 0, nullptr, 1), VECTOR_STATUS_NULL);
}

TEST(VectorExt, VecBulkInsertBounds) {
	vector vec = {
		.data = nullptr,
		.count = 0,
		._type_size = sizeof(char),
		._alloc_count = 0,
	};

	char elements[] = { element0, element1, element2 };

	EXPECT_EQ(vec_bulk_insert(&vec, 1, elements, 3), VECTOR_STATUS_BOUNDS);
	EXPECT_EQ(vec.data, nullptr);
	EXPECT_EQ(vec.count, 0);
}

TEST(VectorExt, VecBulkInsertOk) {
	vector vec = {
		.data = nullptr,
		.count = 0,
		._type_size = sizeof(char),
		._alloc_count = 0,
	};

	EXPECT_EQ(vec_bulk_insert(&vec, 0, elements0, 3), VECTOR_STATUS_OK);
	EXPECT_NE(vec.data, nullptr);
	EXPECT_EQ(*((char *)vec.data), element0);
	EXPECT_EQ(*((char *)vec.data + 1), element1);
	EXPECT_EQ(*((char *)vec.data + 2), element2);
	EXPECT_EQ(vec.count, 3);

	EXPECT_EQ(vec_bulk_insert(&vec, 0, elements1, 3), VECTOR_STATUS_OK);
	EXPECT_EQ(*((char *)vec.data), element2);
	EXPECT_EQ(*((char *)vec.data + 1), element0);
	EXPECT_EQ(*((char *)vec.data + 2), element1);
	EXPECT_EQ(vec.count, 6);

	// Verify other elements
	EXPECT_EQ(*((char *)vec.data + 3), element0);
	EXPECT_EQ(*((char *)vec.data + 4), element1);
	EXPECT_EQ(*((char *)vec.data + 5), element2);

	free(vec.data);
}

TEST(VectorExt, VecEraseNull) {
	vector *vec = nullptr;

	EXPECT_EQ(vec_bulk_erase(vec, 0, nullptr, 1), VECTOR_STATUS_NULL);
}

TEST(VectorExt, VecEraseBounds) {
	vector vec = {
		.data = malloc(sizeof(char) * 2),
		.count = 2,
		._type_size = sizeof(char),
		._alloc_count = 2,
	};

	*((char *)vec.data) = element0;
	*((char *)vec.data + 1) = element1;

	EXPECT_EQ(vec_bulk_erase(&vec, 2, nullptr, 2), VECTOR_STATUS_BOUNDS);
	EXPECT_EQ(vec.count, 2);

	EXPECT_EQ(vec_bulk_erase(&vec, 1, nullptr, 2), VECTOR_STATUS_BOUNDS);
	EXPECT_EQ(vec.count, 2);

	EXPECT_EQ(vec_bulk_erase(&vec, 0, nullptr, 2), VECTOR_STATUS_OK);
	EXPECT_EQ(vec.count, 0);

	free(vec.data);
}

TEST(VectorExt, VecEraseOk) {
	vector vec = {
		.data = malloc(sizeof(char) * 9),
		.count = 9,
		._type_size = sizeof(char),
		._alloc_count = 9,
	};

	// elements0
	*((char *)vec.data) = element0;
	*((char *)vec.data + 1) = element1;
	*((char *)vec.data + 2) = element2;

	// random junk
	*((char *)vec.data + 3) = element2;
	*((char *)vec.data + 4) = element1;
	*((char *)vec.data + 5) = element0;

	// elements1
	*((char *)vec.data + 6) = element2;
	*((char *)vec.data + 7) = element0;
	*((char *)vec.data + 8) = element1;

	char buffer[] = { 12, 34, 56 };
	EXPECT_EQ(vec_bulk_erase(&vec, 0, buffer, 3), VECTOR_STATUS_OK);
	EXPECT_EQ(memcmp(buffer, elements0, 3), 0);
	EXPECT_EQ(vec.count, 6);

	// Verify elements
	EXPECT_EQ(*((char *)vec.data), element2);
	EXPECT_EQ(*((char *)vec.data + 1), element1);
	EXPECT_EQ(*((char *)vec.data + 2), element0);
	EXPECT_EQ(*((char *)vec.data + 3), element2);
	EXPECT_EQ(*((char *)vec.data + 4), element0);
	EXPECT_EQ(*((char *)vec.data + 5), element1);

	EXPECT_EQ(vec_bulk_erase(&vec, 3, buffer, 3), VECTOR_STATUS_OK);
	EXPECT_EQ(memcmp(buffer, elements1, 3), 0);
	EXPECT_EQ(vec.count, 3);

	// Verify elements
	EXPECT_EQ(*((char *)vec.data), element2);
	EXPECT_EQ(*((char *)vec.data + 1), element1);
	EXPECT_EQ(*((char *)vec.data + 2), element0);

	free(vec.data);
}
