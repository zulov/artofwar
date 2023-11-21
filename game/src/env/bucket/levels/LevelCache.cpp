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
	std::vector<std::pair<Urho3D::IntVector2, short>> temp;

	const auto step = maxDistance / RES_SEP_DIST;
	for (int i = 1; i < RES_SEP_DIST; ++i) {
		levels[i] = getEnvIndexs(step * i, levels[i - 1], temp);
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
                                         std::vector<std::pair<Urho3D::IntVector2, short>>& temp) const {
	radius /= calculator->getFieldSize();
	radius *= radius;
	temp.clear();
	temp.reserve(prev.indexes->size());
	short maxShift = 0;
	for (short i = 0; i < RES_SEP_DIST; ++i) {
		const short x = i + 1;
		const auto sqI = i * i;
		//j=0
		if (sqI < radius) {
			maxShift = x;
			temp.emplace_back(Urho3D::IntVector2(0, -x), -1);
			temp.emplace_back(Urho3D::IntVector2(-x, 0), -1);
		} else {
			break;
		}
		for (short j = 0; j < RES_SEP_DIST; ++j) {
			if (sqI + j * j < radius) {
				const short y = j + 1;
				temp.emplace_back(Urho3D::IntVector2(-x, y), -1);
				temp.emplace_back(Urho3D::IntVector2(-x, -y), -1);
			} else {
				break;
			}
		}
	}
	if (temp.size() * 2 + 1 == prev.shifts->size()) {
		return prev;
	}
	std::ranges::reverse(temp);
	for (auto& pair : temp) {
		pair.second = calculator->getNotSafeIndexClose(pair.first.x_, pair.first.y_);
	}

	std::ranges::sort(temp, [](const auto& left, const auto& right) {
		return left.second < right.second;
	});
	LevelCacheValue result;
	result.init(temp.size() * 2 + 1);
	for (const auto& value : temp) {
		result.push(value.first, value.second);
	}
	result.push({}, 0);

	const auto size = temp.size() - 2;
	for (int i = size; i >= 0; --i) {
		auto& val = temp[i];
		result.push(-(val.first), -(val.second));
	}
	result.maxShift = maxShift;
	return result;
}
