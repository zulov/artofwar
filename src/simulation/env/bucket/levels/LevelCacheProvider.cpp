#include "LevelCacheProvider.h"

#include "LevelCache.h"
#include "utils/DeleteUtils.h"

std::vector<LevelCache*> LevelCacheProvider::caches;

LevelCacheProvider::~LevelCacheProvider() {
	clear_vector(caches);
}

LevelCache* LevelCacheProvider::get(unsigned short resolution, float maxDistance, GridCalculator* calculator) {
	for (auto cache : caches) {
		if (cache->getResolution() == resolution && cache->getMaxDistance() == maxDistance) {
			return cache;
		}
	}
	auto* const cache = new LevelCache(maxDistance, calculator);
	caches.push_back(cache);
	return cache;
}
