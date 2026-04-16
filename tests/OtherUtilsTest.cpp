#include "pch.h"

#include "utils/OtherUtils.h"

class OtherUtilsFixture : public ::testing::Test {};

// --- key ---

TEST_F(OtherUtilsFixture, KeyZeroZero) {
	EXPECT_EQ(key(0, 0), 0u);
}

TEST_F(OtherUtilsFixture, KeyFirstOnly) {
	// i=1 shifted left 32 bits
	EXPECT_EQ(key(1, 0), (size_t)1 << 32);
}

TEST_F(OtherUtilsFixture, KeySecondOnly) {
	EXPECT_EQ(key(0, 1), 1u);
}

TEST_F(OtherUtilsFixture, KeyCombined) {
	size_t expected = ((size_t)5 << 32) | (unsigned int)10;
	EXPECT_EQ(key(5, 10), expected);
}

TEST_F(OtherUtilsFixture, KeyNegativeJ) {
	// j=-1 cast to unsigned int = 0xFFFFFFFF
	size_t expected = ((size_t)1 << 32) | 0xFFFFFFFFu;
	EXPECT_EQ(key(1, -1), expected);
}

TEST_F(OtherUtilsFixture, KeyDistinct) {
	// key(1,2) != key(2,1)
	EXPECT_NE(key(1, 2), key(2, 1));
}

TEST_F(OtherUtilsFixture, KeyLargeValues) {
	size_t expected = ((size_t)INT_MAX << 32) | (unsigned int)INT_MAX;
	EXPECT_EQ(key(INT_MAX, INT_MAX), expected);
}

// --- castC ---

enum class TestEnum : char { A = 0, B = 1, C = 5 };

TEST_F(OtherUtilsFixture, CastCBasic) {
	EXPECT_EQ(castC(TestEnum::A), 0);
	EXPECT_EQ(castC(TestEnum::B), 1);
	EXPECT_EQ(castC(TestEnum::C), 5);
}
