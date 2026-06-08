#include "pch.h"

#include "math/RandGen.h"
#include "../game/src/math/RandGen.cpp"

class RandGenFixture : public ::testing::Test {
protected:
	void SetUp() override {
		RandGen::init(false);
	}
};

TEST_F(RandGenFixture, InitFalseProducesRepeatableFloatSequence) {
	auto first = RandGen::nextRand(RandFloatType::AI, 1.f);
	auto second = RandGen::nextRand(RandFloatType::AI, 1.f);

	RandGen::init(false);

	EXPECT_FLOAT_EQ(RandGen::nextRand(RandFloatType::AI, 1.f), first);
	EXPECT_FLOAT_EQ(RandGen::nextRand(RandFloatType::AI, 1.f), second);
}

TEST_F(RandGenFixture, ResetFalseReplaysSequenceWithoutRegeneratingData) {
	auto first = RandGen::nextRand(RandFloatType::RESOURCE_ROTATION, 1.f);
	RandGen::nextRand(RandFloatType::RESOURCE_ROTATION, 1.f);

	RandGen::reset(false);

	EXPECT_FLOAT_EQ(RandGen::nextRand(RandFloatType::RESOURCE_ROTATION, 1.f), first);
}

TEST_F(RandGenFixture, FloatTypesKeepIndependentIndexes) {
	auto aiFirst = RandGen::nextRand(RandFloatType::AI, 10.f);
	auto aiSecond = RandGen::nextRand(RandFloatType::AI, 10.f);
	auto otherFirst = RandGen::nextRand(RandFloatType::OTHER, 10.f);
	auto otherSecond = RandGen::nextRand(RandFloatType::OTHER, 10.f);

	EXPECT_FLOAT_EQ(aiFirst, otherFirst);
	EXPECT_FLOAT_EQ(aiSecond, otherSecond);
}

TEST_F(RandGenFixture, IntRandomValuesRespectProvidedUpperBound) {
	for (int i = 0; i < 32; ++i) {
		auto value = RandGen::nextRand(RandIntType::SAVE, 7);
		EXPECT_GE(value, 0);
		EXPECT_LT(value, 7);
	}
}

TEST_F(RandGenFixture, IntRandomReturnsZeroForNonPositiveBound) {
	// max <= 0 must not hit the modulo (mod-by-zero is UB); it returns 0 instead.
	EXPECT_EQ(RandGen::nextRand(RandIntType::SAVE, 0), 0);
	EXPECT_EQ(RandGen::nextRand(RandIntType::SAVE, -5), 0);
}
