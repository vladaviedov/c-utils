extern "C" {
#define restrict
#include "../include/ustring.h"
}

#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>

namespace ustring_tests {

	static const uchar empty_string[] = { 0 };
	static const uchar test_string[]
		= { U'こ', U'ん', U'に', U'ち', U'は', U'世', U'界', 0 };

	static const uchar unterminated_string[]
		= { U'h', U'e', U'l', U'l', U'o', 0x1234, 0x5678, 0 };
	static const uchar terminated_string[]
		= { U'h', U'e', U'l', U'l', U'o', 0 };
	static const uchar null_padded_string[]
		= { U'h', U'e', U'l', U'l', U'o', 0, 0, 0 };

	TEST(Unicode, Ustrlen) {
		EXPECT_EQ(ustrlen(empty_string), 0);
		EXPECT_EQ(ustrlen(test_string), 7);
	}

	TEST(Unicode, Ustrnlen) {
		EXPECT_EQ(ustrnlen(empty_string, 5), 0);
		EXPECT_EQ(ustrnlen(test_string, 5), 5);
		EXPECT_EQ(ustrnlen(test_string, 10), 7);
		EXPECT_EQ(ustrnlen(unterminated_string, 5), 5);
	}

	TEST(Unicode, Ustrdup) {
		uchar *copy_str = ustrdup(test_string);
		EXPECT_EQ(memcmp(copy_str, test_string, sizeof(test_string)), 0);
		free(copy_str);
	}

	TEST(Unicode, Ustrndup) {
		uchar *copy_str = ustrndup(unterminated_string, 5);
		EXPECT_EQ(memcmp(copy_str, terminated_string, sizeof(terminated_string)), 0);
		free(copy_str);
	}

	TEST(Unicode, Ustrcpy) {
		uchar copy_buf[8];
		ustrcpy(copy_buf, test_string);

		EXPECT_EQ(memcmp(copy_buf, test_string, sizeof(test_string)), 0);
	}

	TEST(Unicode, Ustrncpy) {
		uchar copy_buf1[5];
		ustrncpy(copy_buf1, unterminated_string, 5);
		EXPECT_EQ(memcmp(copy_buf1, unterminated_string, sizeof(uchar) * 5), 0);

		uchar copy_buf2[8];
		ustrncpy(copy_buf2, terminated_string, 8);
		EXPECT_EQ(memcmp(copy_buf2, null_padded_string, sizeof(null_padded_string)), 0);
	}

} // namespace ustring_tests
