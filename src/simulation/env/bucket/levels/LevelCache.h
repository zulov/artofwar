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
private:
	std::vector<short>* getEnvIndexs(float radius, std::vector<short>* prev, std::vector<short>& temp) const;
	float maxDistance;
	float invDiff;

	GridCalculator * calculator;
	std::vector<short>* levelsCache[RES_SEP_DIST];//TODO mem opt mozna to jako jeden vector i podzelisc na spany
};
