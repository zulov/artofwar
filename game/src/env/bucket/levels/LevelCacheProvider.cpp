#include "LevelCacheProvider.h"

#include "LevelCache.h"
#include "utils/DeleteUtils.h"

std::vector<LevelCache*> LevelCacheProvider::caches;

LevelCacheProvider::~LevelCacheProvider() {
	clear_vector(caches);
}

LevelCache* LevelCacheProvider::get(unsigned short resolution, bool initCords, float maxDistance, GridCalculator* calculator) {
	for (const auto cache : caches) {
		if (cache->getResolution() == resolution && cache->getMaxDistance() == maxDistance) {
			if (initCords) {
				cache->initCordsFn();
			}
			return cache;
		}
	}
	auto* const toReturn = new LevelCache(maxDistance, initCords, calculator);
	caches.push_back(toReturn);
	return toReturn;
}
