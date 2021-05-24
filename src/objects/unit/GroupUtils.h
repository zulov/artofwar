#pragma once
#include <vector>
#include <objects/unit/Unit.h>
#include <Game.h>
#include "simulation/env/Environment.h"

inline bool addToGroup(std::vector<std::vector<int>>& groupedIndexes, int current) {
	for (auto& groupedIndex : groupedIndexes) {
		const auto isNear = std::ranges::any_of(groupedIndex, [current](int index) {
			return Game::getEnvironment()->isInLocal1and2Area(current, index);
		});
		if (isNear) {
			groupedIndex.push_back(current);
			return true;
		}
	}
	return false;
}

inline std::vector<std::vector<Unit*>> divide(const std::vector<Unit*>& units) {
	std::vector<int> allIndexes;
	allIndexes.reserve(units.size());
	for (auto* unit : units) {
		allIndexes.push_back(unit->getMainBucketIndex());
	}
	std::ranges::sort(allIndexes);
	allIndexes.erase(std::unique(allIndexes.begin(), allIndexes.end()), allIndexes.end());
	std::vector<std::vector<int>> groupedIndexes;

	for (int current : allIndexes) {
		const bool added = addToGroup(groupedIndexes, current);
		if (!added) {
			std::vector<int> temp;
			temp.push_back(current);
			groupedIndexes.push_back(temp);
		}
	}
	//merge
	std::vector<std::vector<int>> newGroupedIndexes;
	for (int i = 0; i < groupedIndexes.size(); ++i) {
		const auto& current = groupedIndexes[i];

		if (current.empty()) { continue; }
		std::vector<int> merged = current;
		for (int j = i + 1; j < groupedIndexes.size(); ++j) {
			auto& next = groupedIndexes[j];
			for (auto value : current) {
				const auto isNear = std::ranges::any_of(next, [value](int index) {
					return Game::getEnvironment()->isInLocal1and2Area(value, index);
				});
				if (isNear) {
					merged.insert(merged.end(), next.begin(), next.end());
					next.clear();
				}
			}
		}
		newGroupedIndexes.push_back(merged);
	}
	groupedIndexes = newGroupedIndexes;
	std::vector<std::vector<Unit*>> result;
	result.reserve(groupedIndexes.size());
	for (auto& groupedIndex : groupedIndexes) {
		std::vector<Unit*> temp;
		//temp.reserve()
		for (auto index : groupedIndex) {
			for (auto value : units) {
				if (value->getMainBucketIndex() == index) {
					temp.push_back(value);
				}
			}
		}

		result.push_back(temp);
	}
	return result;
}

inline Urho3D::Vector2 computeLocalCenter(const std::vector<Unit*>& units) {
	auto localCenter = Urho3D::Vector2::ZERO;
	for (auto* unit : units) {
		const auto pos = unit->getPosition();
		localCenter.x_ += pos.x_;
		localCenter.y_ += pos.z_;
	}
	localCenter /= units.size();
	return localCenter;
}
