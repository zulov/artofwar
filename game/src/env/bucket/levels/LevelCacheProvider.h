#pragma once
#include <vector>

struct GridCalculator;
class LevelCache;

class LevelCacheProvider {
public:
	~LevelCacheProvider();
	static LevelCache* get(unsigned short resolution, float maxDistance, GridCalculator* calculator);
private:
	LevelCacheProvider() = default;
	static std::vector<LevelCache*> caches;
};
