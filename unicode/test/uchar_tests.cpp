extern "C" {
#include "../include/uchar.h"
}

#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>

static const uchar uc_empty[] = { 0 };
static const uint64_t uc_len_empty = 0;
static const char *utf8_empty = "";

static const uchar uc_big_points[]
	= { 'a', 'b', 'c', 0xff000000, 'd', 'e', 0x0011ffff, 0 };

static const uchar uc_ascii[]
	= { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', 0 };
static const uint64_t uc_len_ascii = 11;
static const char *utf8_ascii = "hello world";

static const uchar uc_multibyte1[]
	= { U'п', U'р', U'и', U'в', U'е', U'т', ' ', U'м', U'и', U'р', 0 };
static const uint64_t uc_len_multibyte1 = 10;
static const char *utf8_multibyte1 = "привет мир";

static const uchar uc_multibyte2[]
	= { U'こ', U'ん', U'に', U'ち', U'は', U'世', U'界', 0 };
static const uint64_t uc_len_multibyte2 = 7;
static const char *utf8_multibyte2 = "こんにちは世界";

TEST(Unicode, ParseNull) {
	uchar *result = utf8_parse(nullptr);

	EXPECT_EQ(result, nullptr);
}

TEST(Unicode, ParseEmpty) {
	uchar *result = utf8_parse(utf8_empty);

	ASSERT_NE(result, nullptr);
	EXPECT_EQ(memcmp(result, uc_empty, uc_len_empty + 1), 0);

	free(result);
}

TEST(Unicode, ParseAscii) {
	uchar *result = utf8_parse(utf8_ascii);

	ASSERT_NE(result, nullptr);
	EXPECT_EQ(memcmp(result, uc_ascii, uc_len_ascii + 1), 0);

	free(result);
}

TEST(Unicode, ParseMultibyte) {
	uchar *result1 = utf8_parse(utf8_multibyte1);
	ASSERT_NE(result1, nullptr);
	EXPECT_EQ(memcmp(result1, uc_multibyte1, uc_len_multibyte1 + 1), 0);
	free(result1);

	uchar *result2 = utf8_parse(utf8_multibyte2);
	ASSERT_NE(result2, nullptr);
	EXPECT_EQ(memcmp(result2, uc_multibyte2, uc_len_multibyte2 + 1), 0);
	free(result2);
}

TEST(Unicode, EncodeImpossible) {
	char *result = utf8_encode(uc_big_points);

	EXPECT_EQ(result, nullptr);
}

TEST(Unicode, EncodeNull) {
	char *result = utf8_encode(nullptr);

	EXPECT_EQ(result, nullptr);
}

TEST(Unicode, EncodeEmpty) {
	char *result = utf8_encode(uc_empty);

	ASSERT_NE(result, nullptr);
	EXPECT_STREQ(result, utf8_empty);

	free(result);
}

TEST(Unicode, EncodeAscii) {
	char *result = utf8_encode(uc_ascii);

	ASSERT_NE(result, nullptr);
	EXPECT_STREQ(result, utf8_ascii);

	free(result);
}

TEST(Unicode, EncodeMultibyte) {
	char *result1 = utf8_encode(uc_multibyte1);
	ASSERT_NE(result1, nullptr);
	EXPECT_STREQ(result1, utf8_multibyte1);
	free(result1);

	char *result2 = utf8_encode(uc_multibyte2);
	ASSERT_NE(result2, nullptr);
	EXPECT_STREQ(result2, utf8_multibyte2);
	free(result2);
}
