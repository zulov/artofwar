#include "pch.h"

#include "player/Resources.h"
#include "player/Resources.cpp"

const std::vector<Building*>& Possession::getBuildings() {
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
