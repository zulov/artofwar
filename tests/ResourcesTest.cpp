#include "pch.h"

#include "database/db_struct.h"
#include "player/Resources.h"
#include "player/Resources.cpp"

const std::vector<Building*>& Possession::getBuildings() const {
	static const std::vector<Building*> buildings;
	return buildings;
}

db_building_level* Building::getLevel() const {
	return nullptr;
}

class ResourcesFixture : public ::testing::Test {
protected:
	Resources resources;
};

TEST_F(ResourcesFixture, FoodDecayKeepsFractionalLoss) {
	resources.setValue(5.f, 0.f, 0.f, 0.f);

	resources.updateMonth();

	EXPECT_FLOAT_EQ(resources.getLastFoodLost(), 0.5f);
	EXPECT_FLOAT_EQ(resources.getValue(ResourceType::FOOD), 4.5f);
}

TEST_F(ResourcesFixture, ZeroFoodDoesNotDecay) {
	resources.setValue(0.f, 0.f, 0.f, 0.f);

	resources.updateMonth();

	EXPECT_FLOAT_EQ(resources.getLastFoodLost(), 0.f);
	EXPECT_FLOAT_EQ(resources.getValue(ResourceType::FOOD), 0.f);
}

TEST_F(ResourcesFixture, ResourceReductionCannotMakeValueNegative) {
	resources.setValue(5.f, 0.f, 0.f, 0.f);
	db_with_cost cost(5, 0, 0, 0);

	EXPECT_TRUE(resources.reduce(&cost));
	EXPECT_FLOAT_EQ(resources.getValue(ResourceType::FOOD), 0.f);
}

TEST_F(ResourcesFixture, InsufficientResourceReductionDoesNotChangeValues) {
	resources.setValue(5.f, 0.f, 0.f, 0.f);
	db_with_cost cost(6, 0, 0, 0);

	EXPECT_FALSE(resources.reduce(&cost));
	EXPECT_FLOAT_EQ(resources.getValue(ResourceType::FOOD), 5.f);
}

TEST_F(ResourcesFixture, InvalidInitialResourceValuesAreNormalized) {
	resources.setValue(-5.f, -1.f, 0.f, 0.f);

	EXPECT_FLOAT_EQ(resources.getValue(ResourceType::FOOD), 0.f);
	EXPECT_FLOAT_EQ(resources.getValue(ResourceType::WOOD), 0.f);
}

TEST_F(ResourcesFixture, MonthlyDecayRecoversInvalidNegativeFood) {
	resources.addIncome(cast(ResourceType::FOOD), -5.f);

	resources.updateMonth();

	EXPECT_FLOAT_EQ(resources.getLastFoodLost(), 0.f);
	EXPECT_FLOAT_EQ(resources.getValue(ResourceType::FOOD), 0.f);
}

TEST_F(ResourcesFixture, TechnologyCanReduceFoodDecayRate) {
	resources.setValue(5.f, 0.f, 0.f, 0.f);
	resources.setTechnologyModifiers(0.04f, Resources::DEFAULT_GOLD_GAIN_RATE,
			Resources::DEFAULT_STONE_REFINE_BONUS, Resources::DEFAULT_GOLD_REFINE_BONUS,
			Resources::DEFAULT_FOOD_STORAGE_MULTIPLIER, Resources::DEFAULT_GOLD_STORAGE_MULTIPLIER);

	resources.updateMonth();

	EXPECT_FLOAT_EQ(resources.getLastFoodLost(), 0.2f);
	EXPECT_FLOAT_EQ(resources.getValue(ResourceType::FOOD), 4.8f);
}

TEST_F(ResourcesFixture, TechnologyCanDisableFoodDecay) {
	resources.setValue(5.f, 0.f, 0.f, 0.f);
	resources.setTechnologyModifiers(0.f, Resources::DEFAULT_GOLD_GAIN_RATE,
			Resources::DEFAULT_STONE_REFINE_BONUS, Resources::DEFAULT_GOLD_REFINE_BONUS,
			Resources::DEFAULT_FOOD_STORAGE_MULTIPLIER, Resources::DEFAULT_GOLD_STORAGE_MULTIPLIER);

	resources.updateMonth();

	EXPECT_FLOAT_EQ(resources.getLastFoodLost(), 0.f);
	EXPECT_FLOAT_EQ(resources.getValue(ResourceType::FOOD), 5.f);
}

TEST_F(ResourcesFixture, GatheredResourcesUpdateCumulativeIncome) {
	resources.addGathered(cast(ResourceType::STONE), 4.f);

	EXPECT_FLOAT_EQ(resources.getValue(ResourceType::STONE), 4.f);
	EXPECT_FLOAT_EQ(resources.getSumValues()[cast(ResourceType::STONE)], 4.f);
}

TEST_F(ResourcesFixture, PassiveIncomeUpdatesCumulativeIncome) {
	resources.addGathered(cast(ResourceType::GOLD), 4.f);
	resources.addIncome(cast(ResourceType::GOLD), 0.4f);

	EXPECT_FLOAT_EQ(resources.getValue(ResourceType::GOLD), 4.4f);
	EXPECT_FLOAT_EQ(resources.getSumValues()[cast(ResourceType::GOLD)], 4.4f);
	EXPECT_FLOAT_EQ(resources.getGatherSpeed(ResourceType::GOLD), 0.f);
}
