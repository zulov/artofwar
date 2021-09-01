#include "LevelCache.h"

#include <algorithm>
#include <iostream>
#include "simulation/env/GridCalculator.h"

LevelCache::LevelCache(float maxDistance, bool initCords, GridCalculator* calculator)
	: maxDistance(maxDistance), invDiff(RES_SEP_DIST / maxDistance), calculator(calculator) {

	levelsCache[0] = new std::vector<short>(1);
	std::vector<short> temp;
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		levelsCache[i] = getEnvIndexs(maxDistance / RES_SEP_DIST * i, levelsCache[i - 1], temp);
	}

	levelsCacheCords[0] = nullptr;

	if (initCords) {
		initCordsFn();
	}
}

LevelCache::~LevelCache() {
	delete levelsCache[0];
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		if (levelsCache[i - 1] != levelsCache[i]) {
			delete levelsCache[i];
		}
	}

	if (levelsCacheCords[0] != nullptr) {
		delete levelsCacheCords[0];
		for (int i = 1; i < RES_SEP_DIST; ++i) {
			if (levelsCacheCords[i - 1] != levelsCacheCords[i]) {
				delete levelsCacheCords[i];
			}
		}
	}
}

std::vector<short>* LevelCache::get(float radius) const {
	const int index = radius * invDiff;
	if (index < RES_SEP_DIST) {
		return levelsCache[index];
	}
	return levelsCache[RES_SEP_DIST - 1];
}

std::vector<Urho3D::IntVector2>* LevelCache::getCords(float radius) const {
	assert(levelsCacheCords[0] != nullptr);
	const int index = radius * invDiff;
	if (index < RES_SEP_DIST) {
		return levelsCacheCords[index];
	}
	return levelsCacheCords[RES_SEP_DIST - 1];
}

std::pair<std::vector<short>*, std::vector<Urho3D::IntVector2>*> LevelCache::getBoth(float radius) const {
	assert(levelsCacheCords[0] != nullptr);
	const int index = radius * invDiff;
	if (index < RES_SEP_DIST) {
		assert(levelsCache[index]->size() == levelsCacheCords[index]->size());
		return {levelsCache[index], levelsCacheCords[index]};
	}
	assert(levelsCache[RES_SEP_DIST - 1]->size() == levelsCacheCords[RES_SEP_DIST - 1]->size());
	return {levelsCache[RES_SEP_DIST - 1], levelsCacheCords[RES_SEP_DIST - 1]};
}

void LevelCache::initCordsFn() {
	if (levelsCacheCords[0] == nullptr) {
		levelsCacheCords[0] = new std::vector<Urho3D::IntVector2>();
		levelsCacheCords[0]->push_back(Urho3D::IntVector2::ZERO);
		auto prev = levelsCacheCords[0]; //TODO perf tego nie robic dla ca³osci
		for (int i = 1; i < RES_SEP_DIST; ++i) {
			if (levelsCache[i - 1] != levelsCache[i]) {
				prev = new std::vector<Urho3D::IntVector2>();
				prev->reserve(levelsCache[i]->size());
				for (auto value : *levelsCache[i]) {
					prev->push_back(calculator->getShiftCords(value));
				}
			}
			levelsCacheCords[i] = prev;
		}
	}
}

std::vector<short>* LevelCache::getEnvIndexs(float radius, std::vector<short>* prev, std::vector<short>& temp) const {
	radius *= radius;
	radius /= calculator->getFieldSize() * calculator->getFieldSize();
	temp.clear();
	temp.reserve(prev->size());
	for (short i = 0; i < RES_SEP_DIST; ++i) {
		const short x = i + 1;
		for (short j = 0; j < RES_SEP_DIST; ++j) {
			if (i * i + j * j < radius) {
				const short y = j + 1;
				temp.push_back(calculator->getNotSafeIndexClose(-x, y));
				temp.push_back(calculator->getNotSafeIndexClose(-x, -y));
			} else {
				break;
			}
		}
		//j=0
		if (i * i < radius) {
			temp.push_back(calculator->getNotSafeIndexClose(0, -x));
			temp.push_back(calculator->getNotSafeIndexClose(-x, 0));
		} else {
			break;
		}
	}
	if (temp.size() * 2 + 1 == prev->size()) {
		return prev;
	}
	std::ranges::reverse(temp);
	std::ranges::sort(temp);
	temp.reserve(temp.size() * 2 + 1);
	temp.push_back(0);

	const auto size = temp.size() - 2;
	for (int i = size; i >= 0; --i) {
		temp.push_back(-temp[i]);
	}

	const auto indexes = new std::vector<short>();
	*indexes = temp;

	return indexes;
}
