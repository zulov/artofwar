#include "pch.h"

#include "utils/TechUtils.h"

namespace {
struct TestEffect {
	unsigned short id;
	TechOperation operation;
	float value;
};
}

TEST(TechnologyOrderingTest, AdditiveEffectsAreAppliedBeforePercentEffectsById) {
	TestEffect percent{1, TechOperation::PERCENT, .2f};
	TestEffect add{2, TechOperation::ADD, 10.f};
	std::vector<const TestEffect*> effects{&percent, &add};

	const auto result = TechUtils::applyEffects(100.f, effects, [](const auto*) { return true; });

	EXPECT_FLOAT_EQ(result, 132.f);
}

TEST(TechnologyOrderingTest, EffectIdProvidesStableOrderWithinOperation) {
	TestEffect first{1, TechOperation::ADD, .1f};
	TestEffect second{2, TechOperation::ADD, .2f};
	std::vector<const TestEffect*> effects{&second, &first};

	EXPECT_TRUE(TechUtils::effectComesBefore(first.operation, first.id, second.operation, second.id));
	EXPECT_FALSE(TechUtils::effectComesBefore(second.operation, second.id, first.operation, first.id));
	EXPECT_FLOAT_EQ(TechUtils::applyEffects(10.f, effects, [](const auto*) { return true; }), 10.3f);
}

TEST(TechnologyOrderingTest, CompoundEffectsAreCalculatedBeforeIntegerConversion) {
	TestEffect first{1, TechOperation::PERCENT, .4f};
	TestEffect second{2, TechOperation::PERCENT, .6f};
	std::vector<const TestEffect*> effects{&second, &first};

	const auto result = TechUtils::applyEffects(7.f, effects, [](const auto*) { return true; });

	EXPECT_FLOAT_EQ(result, 15.68f);
}

TEST(TechnologyOrderingTest, TargetSpecificAttackEffectsFoldIntoEffectiveBonus) {
	TestEffect bonus{1, TechOperation::PERCENT, .15f};
	std::vector<const TestEffect*> effects{&bonus};

	const auto effectiveBonus = TechUtils::applyEffectsToAttackBonus(10.f, 10.f, .2f, effects,
		[](const auto* effect) { return effect->operation == TechOperation::PERCENT; });

	EXPECT_NEAR(effectiveBonus, .38f, 1e-6f);
}

TEST(TechnologyOrderingTest, AnyTargetTagMatchesEveryUnitMatchup) {
	EXPECT_TRUE(TechUtils::matchesUnitTargetTag(TechSourceTag::ANY, TechSourceTag::INFANTRY));
	EXPECT_TRUE(TechUtils::matchesUnitTargetTag(TechSourceTag::ANY, TechSourceTag::WORKER));
}

TEST(TechnologyOrderingTest, SpecificTargetTagOnlyMatchesItsUnitMatchup) {
	EXPECT_TRUE(TechUtils::matchesUnitTargetTag(TechSourceTag::CAVALRY, TechSourceTag::CAVALRY));
	EXPECT_FALSE(TechUtils::matchesUnitTargetTag(TechSourceTag::CAVALRY, TechSourceTag::INFANTRY));
}
