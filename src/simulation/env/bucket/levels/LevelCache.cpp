#include "LevelCache.h"

#include <algorithm>

#include "simulation/env/GridCalculator.h"

LevelCache::LevelCache(unsigned short resolution, float maxDistance, GridCalculator* calculator)
	: resolution(resolution), maxDistance(maxDistance), invDiff(RES_SEP_DIST / maxDistance), calculator(calculator) {

	levelsCache[0] = new std::vector<short>(1); //TODO empty???
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		levelsCache[i] = getEnvIndexs(maxDistance / RES_SEP_DIST * i, levelsCache[i - 1]);
	}
}

LevelCache::~LevelCache() {
	delete levelsCache[0];
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		if (levelsCache[i - 1] != levelsCache[i]) {
			delete levelsCache[i];
		}
	}
}

std::vector<short>* LevelCache::get(float radius) {
	const int index = radius * invDiff;
	if (index < RES_SEP_DIST) {
		return levelsCache[index];
	}
	return levelsCache[RES_SEP_DIST - 1];
}

std::vector<short>* LevelCache::getEnvIndexs(float radius, std::vector<short>* prev) const {
	radius *= radius;
	auto indexes = new std::vector<short>();
	for (short i = 0; i < RES_SEP_DIST; ++i) {
		for (short j = 0; j < RES_SEP_DIST; ++j) {
			if (fieldInCircle(i, j, radius)) {
				const short x = i + 1;
				const short y = j + 1;
				indexes->push_back(calculator->getNotSafeIndex(x, y));
				indexes->push_back(calculator->getNotSafeIndex(x, -y));
				indexes->push_back(calculator->getNotSafeIndex(-x, y));
				indexes->push_back(calculator->getNotSafeIndex(-x, -y));
			} else {
				break;
			}
		}
		if (fieldInCircle(i, 0, radius)) {
			const short x = i + 1;
			indexes->push_back(calculator->getNotSafeIndex(x, 0));
			indexes->push_back(calculator->getNotSafeIndex(0, x));
			indexes->push_back(calculator->getNotSafeIndex(-x, 0));
			indexes->push_back(calculator->getNotSafeIndex(0, -x));
		} else {
			break;
		}
	}
	indexes->push_back(calculator->getNotSafeIndex(0, 0));
	std::sort(indexes->begin(), indexes->end());
	if (std::equal(indexes->begin(), indexes->end(), prev->begin(), prev->end())) {
		delete indexes;
		return prev;
	}
	indexes->shrink_to_fit();
	return indexes;
}


bool LevelCache::fieldInCircle(short i, short j, float radius) const {
	const short x = i * calculator->getFieldSize();
	const short y = j * calculator->getFieldSize();
	return x * x + y * y < radius;
}