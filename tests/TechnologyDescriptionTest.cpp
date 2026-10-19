#include "pch.h"

#include "utils/TechnologyDescription.h"

TEST(TechnologyDescriptionTest, FormatsAdditiveValuesWithSign) {
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechOperation::ADD, 5.f), "+5");
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechOperation::ADD, -2.5f), "-2.5");
}

TEST(TechnologyDescriptionTest, FormatsPercentageValuesAsPercent) {
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechOperation::PERCENT, 0.1f), "+10%");
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechOperation::PERCENT, -0.125f), "-12.5%");
}

TEST(TechnologyDescriptionTest, RemovesFloatingPointNoise) {
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechOperation::ADD, 1.234f), "+1.23");
	EXPECT_EQ(TechnologyDescription::formatEffectValue(TechOperation::PERCENT, 0.f), "0%");
}

TEST(TechnologyDescriptionTest, ProvidesEconomyStatKeys) {
	EXPECT_STREQ(TechnologyDescription::statKey(TechStat::FOOD_DECAY), "tech_stat_food_decay");
	EXPECT_STREQ(TechnologyDescription::statKey(TechStat::GOLD_INTEREST), "tech_stat_gold_interest");
	EXPECT_STREQ(TechnologyDescription::statKey(TechStat::ARMY_UPKEEP), "tech_stat_army_upkeep");
	EXPECT_STREQ(TechnologyDescription::statKey(TechStat::SUPPLY_UPKEEP), "tech_stat_supply_upkeep");
	EXPECT_STREQ(TechnologyDescription::statKey(TechStat::STARVATION_DAMAGE), "tech_stat_starvation_damage");
}
