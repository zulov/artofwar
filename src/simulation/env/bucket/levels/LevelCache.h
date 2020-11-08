#pragma once
#include <vector>

struct GridCalculator;
constexpr char RES_SEP_DIST = 120;

class LevelCache {
public:
	explicit LevelCache(unsigned short resolution, float maxDistance, GridCalculator * calculator);
	~LevelCache();

	unsigned short getResolution() const { return resolution; }
	float getMaxDistance() const { return maxDistance; }
	std::vector<short>* get(float radius);
	std::vector<short>* getEnvIndexs(float radius, std::vector<short>* prev) const;
	bool fieldInCircle(short i, short j, float radius) const;
private:
	float maxDistance;
	float invDiff;
	unsigned short resolution;
	GridCalculator * calculator;
	std::vector<short>* levelsCache[RES_SEP_DIST];
};
