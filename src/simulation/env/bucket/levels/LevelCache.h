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
	std::vector<short>* get(float radius) const;
	std::vector<Urho3D::IntVector2>* getCords(float radius) const;
private:
	std::vector<short>* getEnvIndexs(float radius, std::vector<short>* prev, std::vector<short>& temp) const;
	float maxDistance;
	float invDiff;

	GridCalculator * calculator;
	std::vector<short>* levelsCache[RES_SEP_DIST];
	std::vector<Urho3D::IntVector2>* levelsCacheCords[RES_SEP_DIST];//TODO perf to nie zawsze i tylko dla kilku
	
};
