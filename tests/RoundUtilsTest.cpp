#include "pch.h"

#include <limits>

#include "utils/RoundUtils.h"

TEST(RoundUtilsTest, ConvertsToIntWithNearestIntegerAndHalfAwayFromZero) {
	EXPECT_EQ(RoundUtils::toInt(1.4f), 1);
	EXPECT_EQ(RoundUtils::toInt(1.5f), 2);
	EXPECT_EQ(RoundUtils::toInt(-1.5f), -2);
}

TEST(RoundUtilsTest, ConvertsToIntWithMinimum) {
	EXPECT_EQ(RoundUtils::toInt(-2.5f, 1), 1);
	EXPECT_EQ(RoundUtils::toInt(2.5f, 1), 3);
}

TEST(RoundUtilsTest, ConvertsToUShortAndClampsToRepresentableRange) {
	EXPECT_EQ(RoundUtils::toUShort(12.4f), 12);
	EXPECT_EQ(RoundUtils::toUShort(12.5f), 13);
	EXPECT_EQ(RoundUtils::toUShort(-3.f), 0);
	EXPECT_EQ(RoundUtils::toUShort(70000.f), std::numeric_limits<unsigned short>::max());
}

TEST(RoundUtilsTest, ConvertsToUShortWithMinimum) {
	EXPECT_EQ(RoundUtils::toUShort(0.4f, 1), 1);
	EXPECT_EQ(RoundUtils::toUShort(2.5f, 1), 3);
}

TEST(RoundUtilsTest, ConvertsAndClampsCharTypes) {
	EXPECT_EQ(RoundUtils::toChar(-128.5f), std::numeric_limits<signed char>::lowest());
	EXPECT_EQ(RoundUtils::toChar(127.5f), std::numeric_limits<signed char>::max());
	EXPECT_EQ(RoundUtils::toChar(-5.5f, -2), -2);
	EXPECT_EQ(RoundUtils::toUChar(12.5f), 13);
	EXPECT_EQ(RoundUtils::toUChar(-1.f), 0);
	EXPECT_EQ(RoundUtils::toUChar(300.f), std::numeric_limits<unsigned char>::max());
	EXPECT_EQ(RoundUtils::toUChar(0.1f, 1), 1);
}

TEST(RoundUtilsTest, ConvertsAndClampsShortTypes) {
	EXPECT_EQ(RoundUtils::toShort(-32769.f), std::numeric_limits<short>::lowest());
	EXPECT_EQ(RoundUtils::toShort(32768.f), std::numeric_limits<short>::max());
	EXPECT_EQ(RoundUtils::toShort(-4.5f, -2), -2);
	EXPECT_EQ(RoundUtils::toUInt(-1.f), 0u);
	EXPECT_EQ(RoundUtils::toUInt(4.5f), 5u);
	EXPECT_EQ(RoundUtils::toUInt(4294967296.f), std::numeric_limits<unsigned int>::max());
	EXPECT_EQ(RoundUtils::toUInt(0.f, 2), 2u);
}
TEST(RoundUtilsTest, NaNConvertsToMinimum) {
	EXPECT_EQ(RoundUtils::toInt(std::numeric_limits<float>::quiet_NaN()),
		std::numeric_limits<int>::lowest());
	EXPECT_EQ(RoundUtils::toUShort(std::numeric_limits<float>::quiet_NaN()), 0);
	EXPECT_EQ(RoundUtils::toUShort(std::numeric_limits<float>::quiet_NaN(), 1), 1);
}
