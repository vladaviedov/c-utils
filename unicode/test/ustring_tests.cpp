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

} // namespace ustring_tests
