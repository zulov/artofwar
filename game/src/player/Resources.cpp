#include "Resources.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>

#include "Possession.h"
#include "database/DatabaseCache.h"
#include "math/VectorUtils.h"
#include "objects/building/Building.h"
#include "objects/unit/Unit.h"
#include "player/ArmyUpkeep.h"
#include "utils/RoundUtils.h"
#include "utils/SpanUtils.h"

Resources::Resources() { init(0); }

Resources::Resources(float valueForAll) { init(valueForAll); }

void Resources::init(float valueForAll) {
	resetSpan(values, std::isfinite(valueForAll) ? std::max(0.f, valueForAll) : 0.f);
	resetSpan(gatherSpeeds1s);
	resetSpan(sumGatherSpeed);
	resetSpan(sumValues);
}

bool Resources::reduce(const db_with_cost* costs) {
	if (hasEnough(costs)) {
		for (int i = 0; i < costs->values.size(); ++i) {
			// Keep the resource invariant intact even when floating-point subtraction rounds below zero.
			values[i] = std::max(0.f, values[i] - static_cast<float>(costs->values[i]));
		}
		return true;
	}
	return false;
}

bool Resources::hasEnough(const db_with_cost* costs) const {
	for (int i = 0; i < costs->values.size(); ++i) {
		if (!std::isfinite(values[i]) || values[i] < costs->values[i]) {
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
	const auto normalize = [](float value) {
		return std::isfinite(value) ? std::max(0.f, value) : 0.f;
	};
	values[cast(ResourceType::FOOD)] = normalize(food);
	values[cast(ResourceType::WOOD)] = normalize(wood * 100.f);
	values[cast(ResourceType::STONE)] = normalize(stone * 100.f);
	values[cast(ResourceType::GOLD)] = normalize(gold * 100.f);
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
	int baseFoodStorage = 0;
	int baseGoldStorage = 0;
	for (const auto* building : possession->getBuildings()) {
		if (!building->isReady()) continue;
		const auto* level = building->getLevel();
		baseFoodStorage += level->foodStorage;
		baseGoldStorage += level->goldStorage;
		stoneRefineCapacity += level->stoneRefineCapacity;
		goldRefineCapacity += level->goldRefineCapacity;
	}
	foodStorage = RoundUtils::toInt(baseFoodStorage * foodStorageMultiplier);
	goldStorage = RoundUtils::toInt(baseGoldStorage * goldStorageMultiplier);
	refreshArmyUpkeep(possession);
}

void Resources::setTechnologyModifiers(float newFoodLostRate, float newGoldGainRate, float newStoneRefineBonus,
		float newGoldRefineBonus, float newFoodStorageMultiplier, float newGoldStorageMultiplier,
		float newArmyUpkeepRate, float newStarvationDamageRate) {
	foodLostRate = std::clamp(newFoodLostRate, 0.f, 1.f);
	goldGainRate = std::max(0.f, newGoldGainRate);
	stoneRefineBonus = std::max(0.f, newStoneRefineBonus);
	goldRefineBonus = std::max(0.f, newGoldRefineBonus);
	foodStorageMultiplier = std::max(0.f, newFoodStorageMultiplier);
	goldStorageMultiplier = std::max(0.f, newGoldStorageMultiplier);
	armyUpkeepRate = std::clamp(newArmyUpkeepRate, 0.f, 1.f);
	starvationDamageRate = std::clamp(newStarvationDamageRate, 0.f, 1.f);
}

void Resources::update1s(Possession* possession) {
	std::ranges::copy(sumGatherSpeed, gatherSpeeds1s.begin());
	resetArray(sumGatherSpeed);

	recalculateBuildingState(possession);
	addIncome(cast(ResourceType::STONE), getPotentialStoneRefinement());
	addIncome(cast(ResourceType::GOLD), getPotentialGoldRefinement());
}

void Resources::updateMonth(Possession* possession) {
	auto& food = values[cast(ResourceType::FOOD)];
	if (!std::isfinite(food) || food < 0.f) {
		std::cerr << "[Resource Error] Invalid food value before monthly decay: " << food << "\n";
		food = 0.f;
	}
	if (possession) settleArmyUpkeep(possession);
	lastFoodLost = std::min(food, potentialFoodLost());
	food -= lastFoodLost;
	assert(std::isfinite(food) && food >= 0.f);
}

void Resources::refreshArmyUpkeep(const Possession* possession) {
	armyUpkeep = 0.f;
	for (const auto* unit : possession->getUnits()) {
		if (!unit->isAlive() || unit->getDb()->typeWorker) continue;
		armyUpkeep += static_cast<float>(unit->getDb()->values[cast(ResourceType::FOOD)]) * armyUpkeepRate;
	}
	freeArmyUpkeep = 0.f;
	for (const auto* building : possession->getBuildings()) {
		if (!building->isReady()) continue;
		const auto* dbBuilding = building->getDb();
		if (dbBuilding->typeHome || dbBuilding->typeCenter) {
			freeArmyUpkeep += building->getLevel()->freeArmyUpkeep;
		}
	}
	armyUpkeepDue = std::max(0.f, armyUpkeep - freeArmyUpkeep);
}

void Resources::settleArmyUpkeep(Possession* possession) {
	refreshArmyUpkeep(possession);
	const auto settlement = calculateArmyUpkeep(armyUpkeep, freeArmyUpkeep,
		values[cast(ResourceType::FOOD)], starvationDamageRate);
	// Charge all available upkeep before applying the existing excess-food spoilage.
	values[cast(ResourceType::FOOD)] -= settlement.paid;
	armyUpkeepDue = settlement.due;
	lastArmyUpkeepPaid = settlement.paid;
	lastArmyFoodShortfall = settlement.shortfall;
	lastStarvationDamagePercent = settlement.starvationFraction * 100.f;
	if (settlement.starvationFraction <= 0.f) return;

	// Monthly settlement runs after this tick's transition pass, so lethal starvation is applied next tick.
	// Damage can mark units dead, but their collection remains stable until the next cleanup phase.
	for (auto* unit : possession->getUnits()) {
		if (!unit->isAlive() || unit->getDb()->typeWorker) continue;
		unit->absorbStarvation(unit->getLevel()->maxHp * settlement.starvationFraction);
	}
}

void Resources::updateYear() {
	lastGoldGain = potentialGoldGain();
	addIncome(cast(ResourceType::GOLD), lastGoldGain);
}
