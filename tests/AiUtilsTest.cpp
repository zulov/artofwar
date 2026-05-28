#include "pch.h"

#include "player/ai/AiUtils.h"
#include "../game/src/math/RandGen.cpp"

class AiUtilsFixture : public ::testing::Test {
protected:
	void SetUp() override {
		RandGen::init(false);
	}
};

// --- sampleWeighted ---

TEST_F(AiUtilsFixture, SampleWeightedZeroTotalReturnsZero) {
	std::vector<float> weights = {0.f, 0.f, 0.f};
	EXPECT_EQ(sampleWeighted(weights, 0.f), 0);
}

TEST_F(AiUtilsFixture, SampleWeightedSingleElement) {
	std::vector<float> weights = {5.f};
	EXPECT_EQ(sampleWeighted(weights, 5.f), 0);
}

TEST_F(AiUtilsFixture, SampleWeightedOnlyOneNonZero) {
	std::vector<float> weights = {0.f, 0.f, 3.f};
	// All weight on index 2 — must always return 2
	for (int i = 0; i < 10; ++i) {
		EXPECT_EQ(sampleWeighted(weights, 3.f), 2);
	}
}

TEST_F(AiUtilsFixture, SampleWeightedReturnsValidIndex) {
	std::vector<float> weights = {1.f, 2.f, 3.f};
	float total = 6.f;
	for (int i = 0; i < 50; ++i) {
		int idx = sampleWeighted(weights, total);
		EXPECT_GE(idx, 0);
		EXPECT_LT(idx, 3);
	}
}

// --- biggestWithRand ---

TEST_F(AiUtilsFixture, BiggestWithRandSingleElement) {
	std::vector<float> vals = {5.f};
	EXPECT_EQ(biggestWithRand(vals), 0);
}

TEST_F(AiUtilsFixture, BiggestWithRandAllZeros) {
	std::vector<float> vals = {0.f, 0.f, 0.f};
	EXPECT_EQ(biggestWithRand(vals), 0);
}

TEST_F(AiUtilsFixture, BiggestWithRandNegativesClamped) {
	// Only index 2 has positive weight after clamping
	std::vector<float> vals = {-5.f, -3.f, 10.f};
	for (int i = 0; i < 10; ++i) {
		EXPECT_EQ(biggestWithRand(vals), 2);
	}
}

TEST_F(AiUtilsFixture, BiggestWithRandReturnsValidIndex) {
	std::vector<float> vals = {1.f, 2.f, 3.f};
	for (int i = 0; i < 50; ++i) {
		int idx = biggestWithRand(vals);
		EXPECT_GE(idx, 0);
		EXPECT_LT(idx, 3);
	}
}

// --- lowestWithRand (single) ---

TEST_F(AiUtilsFixture, LowestWithRandSingleElement) {
	std::vector<float> diffs = {5.f};
	EXPECT_EQ(lowestWithRand(std::span<const float>(diffs)), 0);
}

TEST_F(AiUtilsFixture, LowestWithRandPrefersLower) {
	// Index 0 has much lower distance — should be picked most often
	std::vector<float> diffs = {0.01f, 100.f, 100.f};
	int counts[3] = {};
	RandGen::init(false);
	for (int i = 0; i < 100; ++i) {
		counts[lowestWithRand(std::span<const float>(diffs))]++;
	}
	EXPECT_GT(counts[0], counts[1]);
	EXPECT_GT(counts[0], counts[2]);
}

TEST_F(AiUtilsFixture, LowestWithRandEqualDistancesAllSelected) {
	std::vector<float> diffs = {1.f, 1.f, 1.f};
	bool seen[3] = {};
	RandGen::init(false);
	for (int i = 0; i < 100; ++i) {
		seen[lowestWithRand(std::span<const float>(diffs))] = true;
	}
	EXPECT_TRUE(seen[0]);
	EXPECT_TRUE(seen[1]);
	EXPECT_TRUE(seen[2]);
}

// --- lowestWithRand (multi) ---

TEST_F(AiUtilsFixture, LowestWithRandMultiReturnsCorrectCount) {
	std::vector<float> diffs = {1.f, 2.f, 3.f};
	auto result = lowestWithRand(std::span<const float>(diffs), 5);
	EXPECT_EQ(result.size(), 5);
}

TEST_F(AiUtilsFixture, LowestWithRandMultiAllValidIndices) {
	std::vector<float> diffs = {1.f, 2.f, 3.f};
	auto result = lowestWithRand(std::span<const float>(diffs), 10);
	for (int idx : result) {
		EXPECT_GE(idx, 0);
		EXPECT_LT(idx, 3);
	}
}

TEST_F(AiUtilsFixture, LowestWithRandMultiZeroCountReturnsEmpty) {
	std::vector<float> diffs = {1.f, 2.f};
	auto result = lowestWithRand(std::span<const float>(diffs), 0);
	EXPECT_TRUE(result.empty());
}

TEST_F(AiUtilsFixture, LowestWithRandMultiSingleElementAlwaysSame) {
	std::vector<float> diffs = {5.f};
	auto result = lowestWithRand(std::span<const float>(diffs), 3);
	EXPECT_EQ(result.size(), 3);
	for (int idx : result) {
		EXPECT_EQ(idx, 0);
	}
}

TEST_F(AiUtilsFixture, LowestWithRandMultiPrefersLower) {
	std::vector<float> diffs = {0.01f, 100.f, 100.f};
	auto result = lowestWithRand(std::span<const float>(diffs), 50);
	int counts[3] = {};
	for (int idx : result) {
		counts[idx]++;
	}
	EXPECT_GT(counts[0], counts[1]);
	EXPECT_GT(counts[0], counts[2]);
}

TEST_F(AiUtilsFixture, LowestWithRandMultiCanProduceDifferentResults) {
	std::vector<float> diffs = {1.f, 1.f, 1.f};
	auto result = lowestWithRand(std::span<const float>(diffs), 50);
	bool seen[3] = {};
	for (int idx : result) {
		seen[idx] = true;
	}
	EXPECT_TRUE(seen[0]);
	EXPECT_TRUE(seen[1]);
	EXPECT_TRUE(seen[2]);
}
