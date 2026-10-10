#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <objects/resource/ResourceType.h>
#include <span>
#include <string>

#include "scene/load/RuntimeSaveData.h"
#include "simulation/SimGlobals.h"

class Possession;
struct db_with_cost;

class Resources {
public:
	static constexpr float DEFAULT_FOOD_LOST_RATE = 0.1f;
	static constexpr float DEFAULT_STONE_REFINE_BONUS = 0.1f;
	static constexpr float DEFAULT_GOLD_GAIN_RATE = 0.01f;
	static constexpr float DEFAULT_GOLD_REFINE_BONUS = 0.1f;
	static constexpr float DEFAULT_FOOD_STORAGE_MULTIPLIER = 1.f;
	static constexpr float DEFAULT_GOLD_STORAGE_MULTIPLIER = 1.f;

	Resources();
	~Resources() = default;
	void init(float valueForAll);

	explicit Resources(float valueForAll);
	Resources(const Resources&) = delete;

	bool reduce(const db_with_cost* costs);
	bool hasEnough(const db_with_cost* costs) const;
	void addGathered(int id, float value);
	void addIncome(int id, float value);

	std::span<float> getValues() { return values; }
	std::span<float> getGatherSpeeds() { return gatherSpeeds1s; }
	std::span<float> getSumValues() { return sumValues; }

	float getValue(ResourceType rt) const { return values[cast(rt)]; }
	float getGatherSpeed(ResourceType rt) const { return gatherSpeeds1s[cast(rt)]; }

	void setValue(float food, float wood, float stone, float gold);
	ResourcesSaveData saveState(unsigned char player) const;
	void loadState(const ResourcesSaveData& state);
	void recalculateBuildingState(const Possession* possession);
	void setTechnologyModifiers(float foodLostRate, float goldGainRate, float stoneRefineBonus,
			float goldRefineBonus, float foodStorageMultiplier, float goldStorageMultiplier);

	void update1s(Possession* possession);
	void updateMonth();
	void updateYear();

	float getFoodStorage() const { return foodStorage; }
	float getGoldStorage() const { return goldStorage; }

	float getLastFoodLost() const { return lastFoodLost; }
	float potentialFoodLost() const { return std::max(0.f, getValue(ResourceType::FOOD) - foodStorage) * foodLostRate; }

	float getLastGoldGains() const { return lastGoldGain; }
	float potentialGoldGain() const { return std::min(goldStorage, getValue(ResourceType::GOLD)) * goldGainRate; }

	float getStoneRefineCapacity() const { return stoneRefineCapacity; }
	float getPotentialStoneRefinement() const {
		return std::min(getGatherSpeed(ResourceType::STONE), stoneRefineCapacity) * stoneRefineBonus;
	}

	float getPotentialGoldGains() const { return potentialGoldGain(); }
	float getGoldRefineCapacity() const { return goldRefineCapacity; }
	float getPotentialGoldRefinement() const {
		return std::min(getGatherSpeed(ResourceType::GOLD), goldRefineCapacity) * goldRefineBonus;
	}

private:
	std::array<float, RESOURCES_SIZE> values; // TODO wszystie te wartosci trzeba zapisac w savie
	std::array<float, RESOURCES_SIZE> gatherSpeeds1s;
	std::array<float, RESOURCES_SIZE> sumGatherSpeed;
	std::array<float, RESOURCES_SIZE> sumValues;

	int foodStorage = 0;
	float lastFoodLost = 0.f;
	float foodLostRate = DEFAULT_FOOD_LOST_RATE;
	float foodStorageMultiplier = DEFAULT_FOOD_STORAGE_MULTIPLIER;

	float stoneRefineCapacity = 0.f;
	float stoneRefineBonus = DEFAULT_STONE_REFINE_BONUS;

	float goldStorage = 0.f;
	float lastGoldGain = 0.f;
	float goldGainRate = DEFAULT_GOLD_GAIN_RATE;
	float goldStorageMultiplier = DEFAULT_GOLD_STORAGE_MULTIPLIER;

	float goldRefineCapacity = 0.f;
	float goldRefineBonus = DEFAULT_GOLD_REFINE_BONUS;
};
