#include "Resources.h"

#include <numeric>

#include "database/DatabaseCache.h"
#include "math/SpanUtils.h"
#include "math/VectorUtils.h"


Resources::Resources() {
	init(0);
}

Resources::Resources(float valueForAll) {
	init(valueForAll);
}

void Resources::init(float valueForAll) {
	resetSpan(values, valueForAll);
	resetSpan(gatherSpeeds60s);
	resetSpan(gatherSpeeds1s);
	resetSpan(sumGatherSpeed);
	resetSpan(sumValues);
}

bool Resources::reduce(const db_cost* costs) {
	if (hasEnough(costs)) {
		for (int i = 0; i < costs->values.size(); ++i) {
			values[i] -= costs->values[i];
		}
		return true;
	}
	return false;
}

bool Resources::hasEnough(const db_cost* costs) const {
	for (int i = 0; i < costs->values.size(); ++i) {
		if (values[i] < costs->values[i]) {
			return false;
		}
	}
	return true;
}

void Resources::add(int id, float value) {
	values[id] += value;
	sumGatherSpeed[id] += value;
	sumValues[id] += value;
}

std::string Resources::getValues(int precision, int player) const {
	std::string str = "(" + std::to_string(player) + 
		"," + std::to_string((int)(values[0] * precision))+"," + std::to_string((int)(values[1] * precision)) + 
		"," + std::to_string((int)(values[2] * precision))+"," + std::to_string((int)(values[3] * precision)) + 
		"),";

	return str;
}

void Resources::setValue(float food, float wood, float stone, float gold) {
	values[0] = food;
	values[1] = wood;
	values[2] = stone;
	values[3] = gold;
}

void Resources::update1s(Possession* possession) {
	std::ranges::copy(sumGatherSpeed, gatherSpeeds1s.begin());
	resetArray(sumGatherSpeed);

	foodStorage = 0;
	goldStorage = 0;
	stoneRefineCapacity = 0;
	goldRefineCapacity = 0;
	for (const auto building : possession->getBuildings()) {
		const auto level = building->getLevel();
		foodStorage += level->foodStorage;
		goldStorage += level->goldStoreage;
		stoneRefineCapacity += level->stoneRefineCapacity;
		goldRefineCapacity += level->goldRefineCapacity;
	}
	values[2] += getPotentialStoneRefinement();
	values[3] += getPotentialGoldRefinement();
}

void Resources::updateMonth() {
	lastFoodLost = potentialFoodLost();
	values[0] -= lastFoodLost;
	assert(values[0] >= 0);
}

void Resources::updateYear() {
	lastGoldGain = potentialGoldGain();
	values[3] += potentialGoldGain();
}
