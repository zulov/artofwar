#pragma once
#include <vector>

#include "simulation/env/GridCalculator.h"

struct GridCalculator;
constexpr char RES_SEP_DIST = 120;

class LevelCache {
public:
	explicit LevelCache(float maxDistance, GridCalculator * calculator);
	~LevelCache();

	unsigned short getResolution() const { return calculator->getResolution(); }
	float getMaxDistance() const { return maxDistance; }
	std::vector<short>* get(float radius);
	std::vector<short>* getEnvIndexs(float radius, std::vector<short>* prev, std::vector<short>& temp) const;
	bool fieldInCircle(short i, short j, float radius) const;
private:
	float maxDistance;
	float invDiff;

	GridCalculator * calculator;
	std::vector<short>* levelsCache[RES_SEP_DIST];
};
