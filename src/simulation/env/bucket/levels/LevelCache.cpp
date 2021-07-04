#include "LevelCache.h"

#include <algorithm>
#include <iostream>
#include "simulation/env/GridCalculator.h"

LevelCache::LevelCache(float maxDistance, GridCalculator* calculator)
	: maxDistance(maxDistance), invDiff(RES_SEP_DIST / maxDistance), calculator(calculator) {

	levelsCache[0] = new std::vector<short>(1);
	std::vector<short> temp;
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		levelsCache[i] = getEnvIndexs(maxDistance / RES_SEP_DIST * i, levelsCache[i - 1], temp);
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

std::vector<short>* LevelCache::getEnvIndexs(float radius, std::vector<short>* prev, std::vector<short>& temp) const {
	radius *= radius;
	temp.clear();
	temp.reserve(prev->size());
	for (short i = 0; i < RES_SEP_DIST; ++i) {
		const short x = i + 1;
		for (short j = 0; j < RES_SEP_DIST; ++j) {
			if (fieldInCircle(i, j, radius)) {
				const short y = j + 1;
				temp.push_back(calculator->getNotSafeIndexClose(-x, -y));
				temp.push_back(calculator->getNotSafeIndexClose(-x, y));
			} else {
				break;
			}
		}
		if (fieldInCircle(i, 0, radius)) {
			temp.push_back(calculator->getNotSafeIndexClose(-x, 0));
			temp.push_back(calculator->getNotSafeIndexClose(0, -x));
		} else {
			break;
		}
	}
	if (temp.size() * 2 + 1 == prev->size()) {
		return prev;
	}
	temp.reserve(temp.size() * 2 + 1);
	std::ranges::sort(temp);
	temp.push_back(0);

	const auto size = temp.size() - 2;
	for (int i = size; i >= 0; --i) {
		temp.push_back(-temp[i]);
	}

	const auto indexes = new std::vector<short>();
	*indexes = temp;

	return indexes;
}


bool LevelCache::fieldInCircle(short i, short j, float radius) const {
	const short x = i * calculator->getFieldSize();
	const short y = j * calculator->getFieldSize();
	return x * x + y * y < radius;
}
