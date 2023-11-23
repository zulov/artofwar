#include "LevelCache.h"

#include <algorithm>
#include <iostream>
#include <ranges>

#include "env/GridCalculator.h"

LevelCache::LevelCache(float maxDistance, GridCalculator* calculator)
	: maxDistance(maxDistance), invDiff(RES_SEP_DIST / maxDistance), calculator(calculator) {
	levels[0].indexes = new std::vector<short>(1);
	levels[0].shifts = new std::vector<Urho3D::IntVector2>();
	levels[0].shifts->push_back(Urho3D::IntVector2::ZERO);
	std::vector<Urho3D::IntVector2> temp;
	std::vector<Urho3D::IntVector2> tempA;
	std::vector<Urho3D::IntVector2> tempB;
	std::vector<Urho3D::IntVector2> tempC;

	const auto step = maxDistance / RES_SEP_DIST;
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		levels[i] = getEnvIndexs(step * i, levels[i - 1], temp, tempA, tempB, tempC);
	}
}

LevelCache::~LevelCache() {
	levels[0].dispose();
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		if (levels[i - 1].indexes != levels[i].indexes) {
			levels[i].dispose();
		}
	}
}

const LevelCacheValue LevelCache::get(float radius, const Urho3D::IntVector2& centerCords) const {
	int index = radius * invDiff;
	if (index >= RES_SEP_DIST) {
		index = RES_SEP_DIST - 1;
	}
	auto& val = levels[index];

	if (centerCords.x_ - val.maxShift >= 0 && centerCords.x_ + val.maxShift < getResolution() &&
		centerCords.y_ - val.maxShift >= 0 && centerCords.y_ + val.maxShift < getResolution()) {
		return LevelCacheValue(levels[index].indexes);
	}
	return levels[index];
}

const LevelCacheValue LevelCache::get(float radius, int center) const {
	const auto centerCords = calculator->getIndexes(center);
	return get(radius, centerCords);
}

LevelCacheValue LevelCache::getEnvIndexs(float radius, LevelCacheValue& prev,
                                         std::vector<Urho3D::IntVector2>& temp,
                                         std::vector<Urho3D::IntVector2>& tempA,
                                         std::vector<Urho3D::IntVector2>& tempB,
                                         std::vector<Urho3D::IntVector2>& tempC) const {
	radius /= calculator->getFieldSize();
	radius *= radius;
	temp.clear();
	tempA.clear();

	short maxShift = 0;
	for (short i = 0; i < RES_SEP_DIST; ++i) {
		const short x = i + 1;
		const float sqI = i * i;

		tempB.clear();
		tempC.clear();
		for (short j = 0; j < RES_SEP_DIST; ++j) {
			if (sqI + j * j < radius) {
				const short y = j + 1;

				tempB.emplace_back(-x, y);
				tempC.emplace_back(-x, -y);
			} else {
				break;
			}
		}
		temp.insert(temp.end(), tempB.rbegin(), tempB.rend());
		//j=0
		if (sqI < radius) {
			maxShift = x;
			tempA.emplace_back(0, -x);

			temp.emplace_back(-x, 0);
			temp.insert(temp.end(), tempC.begin(), tempC.end());
		} else {
			break;
		}
	}
	const auto desiredSize = (temp.size() + tempA.size()) * 2 + 1;
	if (desiredSize == prev.shifts->size()) {
		return prev;
	}
	//std::ranges::reverse(temp);
	//temp.insert(temp.end(), tempA.rbegin(), tempA.rend());

	LevelCacheValue result(desiredSize);

	for (const auto& v : std::ranges::reverse_view(temp)) {
		result.push(v, calculator->getNotSafeIndexClose(v.x_, v.y_));
	}

	for (const auto& v : std::ranges::reverse_view(tempA)) {
		result.push(v, calculator->getNotSafeIndexClose(v.x_, v.y_));
	}

	result.push({}, 0);

	auto shiftIter = result.shifts->rbegin()+1;
	auto indexIter = result.indexes->rbegin()+1;

	for (; shiftIter != result.shifts->rend(); ++shiftIter, ++indexIter) {
		result.push(-(*shiftIter), -(*indexIter));
	}
	assert(std::ranges::is_sorted(*result.indexes, std::less<short>()));
	assert(desiredSize == result.indexes->size());
	result.maxShift = maxShift;
	return result;
}
