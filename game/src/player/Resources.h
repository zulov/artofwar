#pragma once

#include <array>
#include <string>
#include <span>

#include "utils/defines.h"

struct db_with_cost;
struct db_cost;

class Resources {
public:
	Resources();
	~Resources() = default;
	void init(float valueForAll);

	explicit Resources(float valueForAll);
	Resources(const Resources&) = delete;

	bool reduce(const db_cost* costs);
	bool hasEnough(const db_cost* costs) const;
	void add(int id, float value);

	const std::array<float, RESOURCES_SIZE>& getValues() const { return values; }
	const std::array<float, RESOURCES_SIZE>& getGatherSpeeds() const { return gatherSpeeds1s; }
	const std::array<float, RESOURCES_SIZE>& getSumValues() const { return sumValues; }

	std::string getValues(int precision, int player) const;
	void setValue(int id, float amount);

	void resetStats();
	void updateResourceMonth();
	void updateResourceYear();

	float getFoodStorage() const { return foodStorage; }
	float getGoldStorage() const { return goldStorage; }

	float getLastFoodLost() const { return lastFoodLost; }
	float potentialFoodLost() const { return abs(values[0] - foodStorage) * foodLostRate; }

	float getLastGoldGains() const { return lastGoldGain; }
	float potentialGoldGain() const { return abs(goldStorage - values[3]) * goldGainRate; }

	float getStoneRefineCapacity() const { return 0.f; }
	float getPotentialStoneRefinement() const { return 0.f; }

	float getPotentialGoldGains() const { return 0.f; }
	float getGoldRefineCapacity() const { return goldRefineCapacity; }
	float getPotentialGoldRefinement() const { return 0.f; }

private:
	std::array<float, RESOURCES_SIZE> values;//TODO wszystie te wartosci trzeba zapisac w savie
	std::array<float, RESOURCES_SIZE> gatherSpeeds60s;//to do AI? jakies real speed
	std::array<float, RESOURCES_SIZE> gatherSpeeds1s;
	std::array<float, RESOURCES_SIZE> sumGatherSpeed;
	std::array<float, RESOURCES_SIZE> sumValues;

	float foodStorage = 0.f;
	float lastFoodLost = 0.f;
	float foodLostRate = 0.1f;

	float stoneRefineCapacity = 0.f;
	float stoneRefineValue = 0.f;

	float goldStorage = 0.f;
	float lastGoldGain = 0.f;
	float goldGainRate = 0.01f;
	float goldRefineCapacity = 0.f;


};
