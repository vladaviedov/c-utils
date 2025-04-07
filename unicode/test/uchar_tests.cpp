extern "C" {
#include "../include/uchar.h"
}

#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>

static const uchar uc_null_str[] = { 0 };
static const uchar uc_big_points[]
	= { 'a', 'b', 'c', 0xff000000, 'd', 'e', 0x0011ffff, 0 };

static const uchar uc_ascii[]
	= { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', 0 };
static const char *utf8_ascii = "hello world";

static const uchar uc_multibyte1[]
	= { U'п', U'р', U'и', U'в', U'е', U'т', ' ', U'м', U'и', U'р', 0 };
static const char *utf8_multibyte1 = "привет мир";

static const uchar uc_multibyte2[]
	= { U'こ', U'ん', U'に', U'ち', U'は', U'世', U'界', 0 };
static const char *utf8_multibyte2 = "こんにちは世界";

TEST(Unicode, EncodeImpossible) {
	char *result = utf8_encode(uc_big_points);

	EXPECT_EQ(result, nullptr);
}

TEST(Unicode, EncodeNullString) {
	char *result = utf8_encode(uc_null_str);

	EXPECT_NE(result, nullptr);
	EXPECT_EQ(*result, '\0');
	EXPECT_EQ(strlen(result), 0);

	free(result);
}

TEST(Unicode, EncodeAscii) {
	char *result = utf8_encode(uc_ascii);

	EXPECT_NE(result, nullptr);
	EXPECT_STREQ(result, utf8_ascii);

	free(result);
}

TEST(Unicode, EncodeMultibyte) {
	char *result1 = utf8_encode(uc_multibyte1);
	char *result2 = utf8_encode(uc_multibyte2);

	EXPECT_NE(result1, nullptr);
	EXPECT_NE(result2, nullptr);
	EXPECT_STREQ(result1, utf8_multibyte1);
	EXPECT_STREQ(result2, utf8_multibyte2);

	free(result1);
	free(result2);
}
