extern "C" {
#include "../include/stack.h"
}

#include <cstdlib>
#include <gtest/gtest.h>

static char element0 = '0';
static char element1 = '1';
static char element2 = '2';

TEST(Stack, StackInitOk) {
	stack st = stack_init(sizeof(char));

	EXPECT_EQ(st.count, 0);
	EXPECT_EQ(st._data, nullptr);
}

TEST(Stack, StackNewOk) {
	stack *st = stack_new(sizeof(char));

	EXPECT_NE(st, nullptr);
	EXPECT_EQ(st->count, 0);
	EXPECT_EQ(st->_data, nullptr);

	free(st);
}

TEST(Stack, StackDeinitNullArg) {
	stack *st = nullptr;

	EXPECT_EQ(stack_deinit(st), STACK_STATUS_NULL);
}

TEST(Stack, StackDeinitNullData) {
	stack st = {
		.count = 0,
		._data = nullptr,
		._type_size = 1,
		._alloc_count = 0,
	};

	EXPECT_EQ(stack_deinit(&st), STACK_STATUS_OK);
}

TEST(Stack, StackDeinitOk) {
	stack st = {
		.count = 0,
		._data = malloc(8),
		._type_size = 1,
		._alloc_count = 8,
	};

	EXPECT_EQ(stack_deinit(&st), STACK_STATUS_OK);
}

TEST(Stack, StackDeleteNullArg) {
	stack *st = nullptr;

	EXPECT_EQ(stack_delete(st), STACK_STATUS_NULL);
}

TEST(Stack, StackDeleteNullData) {
	stack *st = (stack *)malloc(sizeof(stack));
	st->_data = nullptr;

	EXPECT_EQ(stack_delete(st), STACK_STATUS_OK);
}

TEST(Stack, StackDeleteOk) {
	stack *st = (stack *)malloc(sizeof(stack));
	st->_data = malloc(8);

	EXPECT_EQ(stack_delete(st), STACK_STATUS_OK);
}

TEST(Stack, StackReserveNull) {
	stack *st = nullptr;

	EXPECT_EQ(stack_reserve(st, 10), STACK_STATUS_NULL);
}

TEST(Stack, StackReserveEmpty) {
	stack st = {
		.count = 0,
		._data = nullptr,
		._type_size = 1,
		._alloc_count = 0,
	};

	EXPECT_EQ(stack_reserve(&st, 6), STACK_STATUS_OK);
	EXPECT_NE(st._data, nullptr);
	EXPECT_GE(st._alloc_count, 6);

	free(st._data);
}

TEST(Stack, StackReserveOk) {
	stack st = {
		.count = 0,
		._data = malloc(8),
		._type_size = 1,
		._alloc_count = 8,
	};

	// Less than allocated
	EXPECT_EQ(stack_reserve(&st, 6), STACK_STATUS_OK);
	EXPECT_GE(st._alloc_count, 6);

	// Same as allocated
	EXPECT_EQ(stack_reserve(&st, 8), STACK_STATUS_OK);
	EXPECT_GE(st._alloc_count, 8);

	// More than allocated
	EXPECT_EQ(stack_reserve(&st, 10), STACK_STATUS_OK);
	EXPECT_GE(st._alloc_count, 10);

	free(st._data);
}

TEST(Stack, StackPushNull) {
	stack *st = nullptr;

	EXPECT_EQ(stack_push(st, nullptr), STACK_STATUS_NULL);
}

TEST(Stack, StackPushOk) {
	stack st = {
		.count = 0,
		._data = nullptr,
		._type_size = sizeof(char),
		._alloc_count = 0,
	};

	EXPECT_EQ(stack_push(&st, &element0), STACK_STATUS_OK);
	EXPECT_NE(st._data, nullptr);
	EXPECT_EQ(*((char *)st._data), element0);
	EXPECT_EQ(st.count, 1);

	EXPECT_EQ(stack_push(&st, &element1), STACK_STATUS_OK);
	EXPECT_EQ(*((char *)st._data + 1), element1);
	EXPECT_EQ(st.count, 2);

	EXPECT_EQ(stack_push(&st, &element2), STACK_STATUS_OK);
	EXPECT_EQ(*((char *)st._data + 2), element2);
	EXPECT_EQ(st.count, 3);

	// Verify previous elements
	EXPECT_EQ(*((char *)st._data), element0);
	EXPECT_EQ(*((char *)st._data + 1), element1);

	free(st._data);
}

TEST(Stack, StackPopNull) {
	stack *st = nullptr;

	EXPECT_EQ(stack_pop(st, nullptr), STACK_STATUS_NULL);
}

TEST(Stack, StackPopEmpty) {
	stack st = {
		.count = 0,
		._data = nullptr,
		._type_size = sizeof(char),
		._alloc_count = 0,
	};

	EXPECT_EQ(stack_pop(&st, nullptr), STACK_STATUS_EMPTY);
}

TEST(Stack, StackPopNullBuffer) {
	stack st = {
		.count = 1,
		._data = malloc(sizeof(char) * 1),
		._type_size = sizeof(char),
		._alloc_count = 1,
	};

	*((char *)st._data) = element0;

	EXPECT_EQ(stack_pop(&st, nullptr), STACK_STATUS_OK);
	EXPECT_EQ(st.count, 0);

	free(st._data);
}

TEST(Stack, StackPopOk) {
	stack st = {
		.count = 3,
		._data = malloc(sizeof(char) * 3),
		._type_size = sizeof(char),
		._alloc_count = 3,
	};

	*((char *)st._data) = element0;
	*((char *)st._data + 1) = element1;
	*((char *)st._data + 2) = element2;

	char buffer = 123;
	EXPECT_EQ(stack_pop(&st, &buffer), STACK_STATUS_OK);
	EXPECT_EQ(buffer, element2);
	EXPECT_EQ(st.count, 2);

	// Verify elements
	EXPECT_EQ(*((char *)st._data), element0);
	EXPECT_EQ(*((char *)st._data + 1), element1);

	EXPECT_EQ(stack_pop(&st, &buffer), STACK_STATUS_OK);
	EXPECT_EQ(buffer, element1);
	EXPECT_EQ(st.count, 1);

	// Verify elements
	EXPECT_EQ(*((char *)st._data), element0);

	free(st._data);
}

TEST(Stack, StackPeekNull) {
	stack *st = nullptr;

	EXPECT_EQ(stack_peek(st), nullptr);
}

TEST(Stack, StackPeekEmpty) {
	stack st = {
		.count = 0,
		._data = nullptr,
		._type_size = sizeof(char),
		._alloc_count = 0,
	};

	EXPECT_EQ(stack_peek(&st), nullptr);
}

TEST(Stack, StackPeekOk) {
	stack st = {
		.count = 2,
		._data = malloc(sizeof(char) * 2),
		._type_size = sizeof(char),
		._alloc_count = 2,
	};

	*((char *)st._data) = element0;
	*((char *)st._data + 1) = element1;

	EXPECT_EQ(*(const char *)stack_peek(&st), element1);

	st.count = 1;
	EXPECT_EQ(*(const char *)stack_peek(&st), element0);

	free(st._data);
}

TEST(Stack, FullTest) {
	stack *st = stack_new(sizeof(char));

	EXPECT_EQ(stack_push(st, &element0), STACK_STATUS_OK);
	EXPECT_EQ(stack_push(st, &element1), STACK_STATUS_OK);
	EXPECT_EQ(stack_push(st, &element2), STACK_STATUS_OK);

	char buffer = 123;

	EXPECT_EQ(*(char *)stack_peek(st), element2);
	EXPECT_EQ(stack_pop(st, &buffer), STACK_STATUS_OK);
	EXPECT_EQ(buffer, element2);

	EXPECT_EQ(*(char *)stack_peek(st), element1);
	EXPECT_EQ(stack_pop(st, &buffer), STACK_STATUS_OK);
	EXPECT_EQ(buffer, element1);

	EXPECT_EQ(stack_push(st, &element2), STACK_STATUS_OK);

	EXPECT_EQ(*(char *)stack_peek(st), element2);
	EXPECT_EQ(stack_pop(st, &buffer), STACK_STATUS_OK);
	EXPECT_EQ(buffer, element2);

	EXPECT_EQ(*(char *)stack_peek(st), element0);
	EXPECT_EQ(stack_pop(st, &buffer), STACK_STATUS_OK);
	EXPECT_EQ(buffer, element0);

	EXPECT_EQ(stack_delete(st), STACK_STATUS_OK);
}
