#pragma once
#include <vector>
#include <objects/unit/Unit.h>
#include <Game.h>
#include "env/Environment.h"
#include "math/VectorUtils.h"
#include <iostream>

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

//TODO upro�ci� dla ma�ych grup
inline std::vector<std::vector<Unit*>> divide(const std::vector<Unit*>& units, bool sort = false) {
	std::vector<int> allIndexes;
	allIndexes.reserve(units.size());
	for (const auto* unit : units) {
		allIndexes.push_back(unit->getMainGridIndex());
	}
	sortAndRemoveDuplicates(allIndexes);
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
		for (const auto index : groupedIndex) {
			for (auto unit : units) {
				if (unit->getMainGridIndex() == index) {
					temp.push_back(unit);
				}
			}
		}

		result.push_back(temp);
	}
	if (sort && units.size() > 2 && result.size() > 1) {
		std::ranges::sort(result, [](const std::vector<Unit*>& a, const std::vector<Unit*>& b) {
			return a.size() > b.size();
		});
	}
	return result;
}

inline Urho3D::Vector2 computeCenter(const std::vector<Unit*>& units) {
	auto localCenter = Urho3D::Vector2::ZERO;
	for (const auto* unit : units) {
		localCenter += unit->getPosition();
	}
	localCenter /= units.size();
	return localCenter;
}
