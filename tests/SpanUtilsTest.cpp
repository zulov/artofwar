#include "pch.h"

#include "utils/SpanUtils.h"

class SpanUtilsFixture : public ::testing::Test {};

// --- minSpan ---

TEST_F(SpanUtilsFixture, MinSpanBasic) {
	std::vector<float> v = {5.f, 3.f, 8.f, 1.f, 4.f};
	EXPECT_FLOAT_EQ(minSpan(v), 1.f);
}

TEST_F(SpanUtilsFixture, MinSpanSingleElement) {
	std::vector<float> v = {42.f};
	EXPECT_FLOAT_EQ(minSpan(v), 42.f);
}

TEST_F(SpanUtilsFixture, MinSpanNegative) {
	std::vector<float> v = {-1.f, -5.f, 0.f, 3.f};
	EXPECT_FLOAT_EQ(minSpan(v), -5.f);
}

// --- maxSpan ---

TEST_F(SpanUtilsFixture, MaxSpanBasic) {
	std::vector<float> v = {5.f, 3.f, 8.f, 1.f};
	EXPECT_FLOAT_EQ(maxSpan(v), 8.f);
}

TEST_F(SpanUtilsFixture, MaxSpanAllSame) {
	std::vector<float> v = {7.f, 7.f, 7.f};
	EXPECT_FLOAT_EQ(maxSpan(v), 7.f);
}

// --- sumSpan ---

TEST_F(SpanUtilsFixture, SumSpanBasic) {
	std::vector<float> v = {1.f, 2.f, 3.f, 4.f};
	EXPECT_FLOAT_EQ(sumSpan(std::span<float>(v)), 10.f);
}

TEST_F(SpanUtilsFixture, SumSpanEmpty) {
	std::vector<float> v;
	EXPECT_FLOAT_EQ(sumSpan(std::span<float>(v)), 0.f);
}

TEST_F(SpanUtilsFixture, SumSpanSingle) {
	std::vector<float> v = {99.f};
	EXPECT_FLOAT_EQ(sumSpan(std::span<float>(v)), 99.f);
}

// --- minAndSumSpan ---

TEST_F(SpanUtilsFixture, MinAndSumSpan) {
	std::vector<float> v = {1.f, 2.f, 3.f};
	// sum=6, min=1, result=7
	EXPECT_FLOAT_EQ(minAndSumSpan(v), 7.f);
}

// --- minSpanSq ---

TEST_F(SpanUtilsFixture, MinSpanSq) {
	std::vector<float> v = {3.f, -2.f, 5.f};
	// min=-2, squared=4
	EXPECT_FLOAT_EQ(minSpanSq(v), 4.f);
}

// --- maxSpanRoot ---

TEST_F(SpanUtilsFixture, MaxSpanRoot) {
	std::vector<float> v = {3.f, 8.f, 1.f};
	// max=8, sqrt(8+1)=sqrt(9)=3
	EXPECT_FLOAT_EQ(maxSpanRoot(v), 3.f);
}

// --- resetSpan ---

TEST_F(SpanUtilsFixture, ResetSpanDefault) {
	std::vector<float> v = {1.f, 2.f, 3.f};
	resetSpan(v);
	for (auto val : v) {
		EXPECT_FLOAT_EQ(val, 0.f);
	}
}

TEST_F(SpanUtilsFixture, ResetSpanCustomValue) {
	std::vector<float> v = {1.f, 2.f, 3.f};
	resetSpan(v, 5.f);
	for (auto val : v) {
		EXPECT_FLOAT_EQ(val, 5.f);
	}
}

// --- validateSpan ---

TEST_F(SpanUtilsFixture, ValidateSpanValid) {
	std::vector<float> v = {1.f, 2.f, 3.f};
	EXPECT_TRUE(validateSpan(0, "test", std::span<const float>(v)));
}

// --- allPositive ---

TEST_F(SpanUtilsFixture, AllPositiveTrue) {
	std::vector<float> v = {0.f, 1.f, 2.f};
	EXPECT_TRUE(allPositive(0, "test", std::span<const float>(v)));
}

// --- zerosSpan ---

TEST_F(SpanUtilsFixture, ZerosSpanAllZero) {
	std::vector<float> v = {0.f, 0.f, 0.f};
	EXPECT_TRUE(zerosSpan(v.data(), 3));
}

// --- asSpan ---

TEST_F(SpanUtilsFixture, AsSpan) {
	std::vector<int> v = {1, 2, 3};
	auto s = asSpan(v);
	EXPECT_EQ(s.size(), 3);
	EXPECT_EQ(s[0], 1);
	EXPECT_EQ(s[2], 3);
}
