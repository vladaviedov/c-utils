extern "C" {
#include "../include/ucwidth.h"
}

#include <gtest/gtest.h>

namespace ucwidth_tests {

	static const uchar single_width_chars[] = { U'k', U'б', U'α', U'ك', U'ｶ' };
	static const uchar east_asian_chars[] = { U'好', U'い', U'한' };
	static const uchar zero_width_chars[] = { 0x350, 0xdd3, 0x1d196 };

	static const uchar empty_str[] = { 0x0 };
	static const uchar with_control_str[]
		= { U'h', U'e', U'l', U'l', U'o', 0x1, 0x0 };
	static const uchar sample_str[]
		= { U'天', U'気', U'が', U'い', U'い', U'か', U'ら', U'、', U'　',
			  U'散', U'歩', U'し', U'ま', U'し', U'ょ', U'う', 0x0 };

	TEST(Unicode, UcwidthControl) {
		// Some C0 char
		EXPECT_EQ(ucwidth(0x1b), -1);
		// Delete char
		EXPECT_EQ(ucwidth(0x7f), -1);
		// Some C1 char
		EXPECT_EQ(ucwidth(0x83), -1);
	}

	TEST(Unicode, UcwidthSingleWidth) {
		for (uint32_t i = 0; i < 5; i++) {
			EXPECT_EQ(ucwidth(single_width_chars[i]), 1);
		}
	}

	TEST(Unicode, UcwidthEastAsian) {
		for (uint32_t i = 0; i < 3; i++) {
			EXPECT_EQ(ucwidth(east_asian_chars[i]), 2);
		}
	}

	TEST(Unicode, UcwidthZeroWidth) {
		for (uint32_t i = 0; i < 3; i++) {
			EXPECT_EQ(ucwidth(zero_width_chars[i]), 0);
		}
	}

	TEST(Unicode, UcwidthMisc) {
		// Null character
		EXPECT_EQ(ucwidth(0), 0);
		// Soft hyphen
		EXPECT_EQ(ucwidth(0xad), 1);
		// Zero width space
		EXPECT_EQ(ucwidth(0x200b), 0);
	}

	TEST(Unicode, UcswidthEmpty) {
		EXPECT_EQ(ucswidth(empty_str, 256), 0);
	}

	TEST(Unicode, UcswidthControl) {
		EXPECT_EQ(ucswidth(with_control_str, 256), -1);
	}

	TEST(Unicode, UcswidthString) {
		EXPECT_EQ(ucswidth(sample_str, 256), 32);
	}

	TEST(Unicode, UcswidthLimited) {
		EXPECT_EQ(ucswidth(sample_str, 7), 14);
	}

} // namespace ucwidth_tests
