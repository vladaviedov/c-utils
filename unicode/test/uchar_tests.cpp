extern "C" {
#include "../include/uchar.h"
}

#include <cstdlib>
#include <cstring>
#include <gtest/gtest.h>

namespace uchar_tests {

	static const uchar uc_empty[] = { 0 };
	static const char *utf8_empty = "";

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

	static const uchar uc_with_invalid_bytes[] = { U's', U't', U'a', U'r', U't',

		// Invalid ASCII bytes
		0xfffd, 0xfffd, 0xfffd, 0xfffd,
		// Out of range
		0xfffd, 0xfffd, 0xfffd, 0xfffd,

		U'e', U'n', U'd', 0 };
	static const uint8_t utf8_with_invalid_bytes[] = { 's', 't', 'a', 'r', 't',

		// Invalid ASCII bytes
		0xc0, 0xc1, 0xf5, 0xff,
		// Out of range
		0xf4, 0x90, 0x90, 0x90,

		'e', 'n', 'd', 0 };

	static const uchar uc_multibyte_errors[] = { U's', U't', U'a', U'r', U't',

		// Continuation bit leader
		0xfffd, 0xfffd, U'c',
		// Truncated middle char
		0xfffd, 0xfffd, U'c',
		// Truncated last char
		0xfffd, 0xfffd, U'c',
		// Overlong point
		0xfffd, 0xfffd, U'c',

		U'e', U'n', U'd', 0 };
	static const uint8_t utf8_multibyte_errors[] = { 's', 't', 'a', 'r', 't',

		// Continuation bit leader
		0xac, 0xbc, 'c',
		// Truncated middle char
		0xe5, 0xb9, 'c',
		// Truncated last char
		0xe9, 0x8a, 'c',
		// Overlong point
		0xc0, 0x80, 'c',

		'e', 'n', 'd', 0 };

	TEST(Unicode, DecodeNull) {
		bool error;
		uchar *result = utf8_decode(nullptr, &error);

		EXPECT_EQ(result, nullptr);
		EXPECT_EQ(error, true);
	}

	TEST(Unicode, DecodeEmpty) {
		bool error;
		uchar *result = utf8_decode(utf8_empty, &error);

		ASSERT_NE(result, nullptr);
		EXPECT_EQ(error, false);
		EXPECT_EQ(memcmp(result, uc_empty, sizeof(uc_empty)), 0);

		free(result);
	}

	TEST(Unicode, DecodeAscii) {
		bool error;
		uchar *result = utf8_decode(utf8_ascii, &error);

		ASSERT_NE(result, nullptr);
		EXPECT_EQ(error, false);
		EXPECT_EQ(memcmp(result, uc_ascii, sizeof(uc_ascii)), 0);

		free(result);
	}

	TEST(Unicode, DecodeMultibyte) {
		bool error;

		uchar *result1 = utf8_decode(utf8_multibyte1, &error);
		ASSERT_NE(result1, nullptr);
		EXPECT_EQ(error, false);
		EXPECT_EQ(memcmp(result1, uc_multibyte1, sizeof(uc_multibyte1)), 0);
		free(result1);

		uchar *result2 = utf8_decode(utf8_multibyte2, &error);
		ASSERT_NE(result2, nullptr);
		EXPECT_EQ(error, false);
		EXPECT_EQ(memcmp(result2, uc_multibyte2, sizeof(uc_multibyte2)), 0);
		free(result2);
	}

	TEST(Unicode, DecodeInvalidChars) {
		bool error;

		uchar *result1
			= utf8_decode((const char *)utf8_with_invalid_bytes, &error);
		ASSERT_NE(result1, nullptr);
		EXPECT_EQ(error, true);
		EXPECT_EQ(memcmp(result1, uc_with_invalid_bytes,
					  sizeof(uc_with_invalid_bytes)),
			0);
		free(result1);

		uchar *result2
			= utf8_decode((const char *)utf8_multibyte_errors, &error);
		ASSERT_NE(result2, nullptr);
		EXPECT_EQ(error, true);
		EXPECT_EQ(
			memcmp(result2, uc_multibyte_errors, sizeof(uc_multibyte_errors)),
			0);
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

} // namespace uchar_tests
