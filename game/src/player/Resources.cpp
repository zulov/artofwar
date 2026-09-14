#include "Resources.h"

#include <numeric>

#include "Possession.h"
#include "database/DatabaseCache.h"
#include "math/VectorUtils.h"
#include "objects/building/Building.h"
#include "utils/SpanUtils.h"

Resources::Resources() { init(0); }

Resources::Resources(float valueForAll) { init(valueForAll); }

void Resources::init(float valueForAll) {
	resetSpan(values, valueForAll);
	resetSpan(gatherSpeeds1s);
	resetSpan(sumGatherSpeed);
	resetSpan(sumValues);
}

bool Resources::reduce(const db_with_cost* costs) {
	if (hasEnough(costs)) {
		for (int i = 0; i < costs->values.size(); ++i) {
			values[i] -= costs->values[i];
		}
		return true;
	}
	return false;
}

bool Resources::hasEnough(const db_with_cost* costs) const {
	for (int i = 0; i < costs->values.size(); ++i) {
		if (values[i] < costs->values[i]) {
			return false;
		}
	}
	return true;
}

void Resources::addGathered(int id, float value) {
	values[id] += value;
	sumGatherSpeed[id] += value;
	sumValues[id] += value;
}

void Resources::addIncome(int id, float value) {
	values[id] += value;
	sumValues[id] += value;
}

void Resources::setValue(float food, float wood, float stone, float gold) {
	values[cast(ResourceType::FOOD)] = food;
	values[cast(ResourceType::WOOD)] = wood;
	values[cast(ResourceType::STONE)] = stone;
	values[cast(ResourceType::GOLD)] = gold;
}

ResourcesSaveData Resources::saveState(unsigned char player) const {
	return {player, gatherSpeeds1s, sumGatherSpeed, sumValues};
}

void Resources::loadState(const ResourcesSaveData& state) {
	gatherSpeeds1s = state.gatherSpeeds1s;
	sumGatherSpeed = state.sumGatherSpeed;
	sumValues = state.sumValues;
}

void Resources::recalculateBuildingState(const Possession* possession) {
	foodStorage = 0;
	goldStorage = 0;
	stoneRefineCapacity = 0;
	goldRefineCapacity = 0;
	for (const auto* building : possession->getBuildings()) {
		const auto* level = building->getLevel();
		foodStorage += level->foodStorage;
		goldStorage += level->goldStorage;
		stoneRefineCapacity += level->stoneRefineCapacity;
		goldRefineCapacity += level->goldRefineCapacity;
	}
}

void Resources::update1s(Possession* possession) {
	std::ranges::copy(sumGatherSpeed, gatherSpeeds1s.begin());
	resetArray(sumGatherSpeed);

	recalculateBuildingState(possession);
	addIncome(cast(ResourceType::STONE), getPotentialStoneRefinement());
	addIncome(cast(ResourceType::GOLD), getPotentialGoldRefinement());
}

void Resources::updateMonth() {
	lastFoodLost = potentialFoodLost();
	values[cast(ResourceType::FOOD)] -= lastFoodLost;
	assert(values[0] >= 0);
}

void Resources::updateYear() {
	lastGoldGain = potentialGoldGain();
	addIncome(cast(ResourceType::GOLD), lastGoldGain);
}
