#pragma once

#include <array>
#include <iostream>
#include <string>
#include <span>

#include "utils/defines.h"

class Possession;
struct db_with_cost;
struct db_cost;

class Resources {
public:
	Resources();
	~Resources() = default;
	void init(float valueForAll);

	explicit Resources(float valueForAll);
	Resources(const Resources&) = delete;

	bool reduce(const db_with_cost* costs);
	bool hasEnough(const db_with_cost* costs) const;
	void add(int id, float value);

	std::span<float> getValues() { return values; }
	std::span<float> getGatherSpeeds() { return gatherSpeeds1s; }
	std::span<float> getSumValues() { return sumValues; }

	std::string getValues(int precision, int player) const;
	void setValue(float food, float wood, float stone, float gold);

	void update1s(Possession* possession);
	void updateMonth();
	void updateYear();

	float getFoodStorage() const { return foodStorage; }
	float getGoldStorage() const { return goldStorage; }

	float getLastFoodLost() const { return lastFoodLost; }
	float potentialFoodLost() const { return std::max(0.f, values[0] - foodStorage) * foodLostRate; }

	float getLastGoldGains() const { return lastGoldGain; }
	float potentialGoldGain() const { return std::max(goldStorage, values[3]) * goldGainRate; }

	float getStoneRefineCapacity() const { return stoneRefineCapacity; }
	float getPotentialStoneRefinement() const {
		return std::min(gatherSpeeds1s[2], stoneRefineCapacity) * stoneRefineBonus;
	}

	float getPotentialGoldGains() const { return potentialGoldGain(); }
	float getGoldRefineCapacity() const { return goldRefineCapacity; }
	float getPotentialGoldRefinement() const { return std::min(gatherSpeeds1s[3], goldRefineCapacity) * goldRefineBonus; }

private:
	std::array<float, RESOURCES_SIZE> values; //TODO wszystie te wartosci trzeba zapisac w savie
	std::array<float, RESOURCES_SIZE> gatherSpeeds60s; //to do AI? jakies real speed
	std::array<float, RESOURCES_SIZE> gatherSpeeds1s;
	std::array<float, RESOURCES_SIZE> sumGatherSpeed;
	std::array<float, RESOURCES_SIZE> sumValues;

	int foodStorage = 0;
	int lastFoodLost = 0;
	float foodLostRate = 0.1f;

	float stoneRefineCapacity = 0.f;
	float stoneRefineBonus = 0.1f;

	float goldStorage = 0.f;
	float lastGoldGain = 0.f;
	float goldGainRate = 0.01f;

	float goldRefineCapacity = 0.f;
	float goldRefineBonus = 0.1f;
};
