#include "pch.h"

#include "utils/TechnologyDescription.h"

TEST(TechnologyDescriptionTest, FormatsAdditiveValuesWithSign) {
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechnologyOperation::ADD, 5.f), "+5");
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechnologyOperation::ADD, -2.5f), "-2.5");
}

TEST(TechnologyDescriptionTest, FormatsPercentageValuesAsPercent) {
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechnologyOperation::PERCENT, 0.1f), "+10%");
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechnologyOperation::PERCENT, -0.125f), "-12.5%");
}

TEST(TechnologyDescriptionTest, RemovesFloatingPointNoise) {
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechnologyOperation::ADD, 1.234f), "+1.23");
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechnologyOperation::PERCENT, 0.f), "0%");
}
