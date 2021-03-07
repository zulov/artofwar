#include "LevelCache.h"

#include <algorithm>
#include <iostream>


#include "simulation/env/GridCalculator.h"

LevelCache::LevelCache(unsigned short resolution, float maxDistance, GridCalculator* calculator)
	: resolution(resolution), maxDistance(maxDistance), invDiff(RES_SEP_DIST / maxDistance), calculator(calculator) {

	levelsCache[0] = new std::vector<short>(1);
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		levelsCache[i] = getEnvIndexs(maxDistance / RES_SEP_DIST * i, levelsCache[i - 1]);
	}
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		if (levelsCache[i - 1] != levelsCache[i]) {
			std::ranges::sort(*(levelsCache[i]));
		} 
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
	indexes->reserve(prev->size());
	for (short i = 0; i < RES_SEP_DIST; ++i) {
		for (short j = 0; j < RES_SEP_DIST; ++j) {
			if (fieldInCircle(i, j, radius)) {
				const short x = i + 1;
				const short y = j + 1;
				auto a = calculator->getNotSafeIndexClose(-x, -y);
				auto b = calculator->getNotSafeIndexClose(-x, y);
				indexes->push_back(a);
				indexes->push_back(b);
				indexes->push_back(-b);
				indexes->push_back(-a);
			} else {
				break;
			}
		}
		if (fieldInCircle(i, 0, radius)) {
			const short x = i + 1;
			auto a = calculator->getNotSafeIndexClose(-x, 0);
			auto b = calculator->getNotSafeIndexClose(0, -x);
			indexes->push_back(a);
			indexes->push_back(b);
			indexes->push_back(-b);
			indexes->push_back(-a);
		} else {
			break;
		}
	}

	indexes->push_back(0);
	if (std::ranges::equal(*indexes, *prev)) {
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
