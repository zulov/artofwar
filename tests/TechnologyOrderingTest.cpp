#include "pch.h"

#include "utils/TechnologyUtils.h"

namespace {
struct TestEffect {
	unsigned short id;
	TechnologyOperation operation;
	float value;
};
}

TEST(TechnologyOrderingTest, AdditiveEffectsAreAppliedBeforePercentEffectsById) {
	TestEffect percent{1, TechnologyOperation::PERCENT, .2f};
	TestEffect add{2, TechnologyOperation::ADD, 10.f};
	std::vector<const TestEffect*> effects{&percent, &add};

	const auto result = TechnologyUtils::applyEffects(100.f, effects, [](const auto*) { return true; });

	EXPECT_FLOAT_EQ(result, 132.f);
}

TEST(TechnologyOrderingTest, EffectIdProvidesStableOrderWithinOperation) {
	TestEffect first{1, TechnologyOperation::ADD, .1f};
	TestEffect second{2, TechnologyOperation::ADD, .2f};
	std::vector<const TestEffect*> effects{&second, &first};

	EXPECT_TRUE(TechnologyUtils::effectComesBefore(first.operation, first.id, second.operation, second.id));
	EXPECT_FALSE(TechnologyUtils::effectComesBefore(second.operation, second.id, first.operation, first.id));
	EXPECT_FLOAT_EQ(TechnologyUtils::applyEffects(10.f, effects, [](const auto*) { return true; }), 10.3f);
}

TEST(TechnologyOrderingTest, CompoundEffectsAreCalculatedBeforeIntegerConversion) {
	TestEffect first{1, TechnologyOperation::PERCENT, .4f};
	TestEffect second{2, TechnologyOperation::PERCENT, .6f};
	std::vector<const TestEffect*> effects{&second, &first};

	const auto result = TechnologyUtils::applyEffects(7.f, effects, [](const auto*) { return true; });

	EXPECT_FLOAT_EQ(result, 15.68f);
}
