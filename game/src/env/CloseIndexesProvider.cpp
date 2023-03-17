#include "CloseIndexesProvider.h"

#include "CloseIndexes.h"
#include "utils/DeleteUtils.h"


std::vector<CloseIndexes*> CloseIndexesProvider::closeIndexeses;

CloseIndexesProvider::~CloseIndexesProvider() {
	clear_vector(closeIndexeses);
}

CloseIndexes* CloseIndexesProvider::get(unsigned short resolution) {
	for (auto cache : closeIndexeses) {
		if (cache->getResolution() == resolution) {
			return cache;
		}
	}
	auto* const cache = new CloseIndexes(resolution);
	closeIndexeses.push_back(cache);
	return cache;
}
