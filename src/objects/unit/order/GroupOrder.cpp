#include "GroupOrder.h"

#include <utility>
#include "FormationOrder.h"
#include "Game.h"
#include "enums/UnitActionType.h"
#include "objects/unit/Unit.h"
#include "simulation/env/Environment.h"
#include "simulation/formation/FormationManager.h"


GroupOrder::GroupOrder(const std::vector<Physical*>& entities, UnitActionType actionType, short id,
                       Physical* toUse, bool append): UnitOrder(id, append, toUse), actionType(actionType) {
	addUnits(entities);
}

GroupOrder::GroupOrder(const std::vector<Physical*>& entities, UnitActionType actionType, short id,
                       Urho3D::Vector2 vector, bool append): UnitOrder(id, append, vector), actionType(actionType) {
	addUnits(entities);
}

GroupOrder::GroupOrder(std::vector<Unit*> entities, UnitActionType actionType, short id,
                       Urho3D::Vector2 vector, bool append): UnitOrder(id, append, vector), actionType(actionType),
                                                             units(std::move(entities)) {
}

void GroupOrder::addUnits(const std::vector<Physical*>& entities) {
	units.reserve(entities.size());
	for (auto unit : entities) {
		this->units.emplace_back(reinterpret_cast<Unit*>(unit));
	}
}

bool GroupOrder::add() {
	switch (actionType) {
	case UnitActionType::ORDER:
		execute();
		break;
	case UnitActionType::FORMATION: {
		int a = 5;
	}
	break;
	default: ;
	}
	return true;
}

void GroupOrder::addCollectAim() {
	transformToFormationOrder();
}

void GroupOrder::addTargetAim() {
	transformToFormationOrder();
}

void GroupOrder::addFollowAim() {
	transformToFormationOrder();
}

void GroupOrder::addChargeAim() {
	transformToFormationOrder();
}

void GroupOrder::addAttackAim() {
	transformToFormationOrder();
}

void GroupOrder::addDefendAim() {
	simpleAction();
}

void GroupOrder::addDeadAim() {
	simpleAction();
}

void GroupOrder::addStopAim() {
	simpleAction();
}

void GroupOrder::simpleAction(ActionParameter& parameter) const {
	const auto action = static_cast<UnitAction>(id);
	for (auto* unit : units) {
		unit->action(action, parameter);
	}
}

void GroupOrder::simpleAction() const {
	const auto action = static_cast<UnitAction>(id);
	for (auto* unit : units) {
		unit->action(action);
	}
}

void GroupOrder::actOnFormation(std::vector<Unit*>& group) const {
	auto opt = Game::getFormationManager()->createFormation(group);
	if (opt.has_value()) {
		if (vector) {
			opt.value()->addOrder(new FormationOrder(opt.value(), id, *vector, append));
		} else {
			opt.value()->addOrder(new FormationOrder(opt.value(), id, toUse, append));
		}
	}
}

void GroupOrder::transformToFormationOrder() const {
	for (auto& group : divide(units)) {
		actOnFormation(group);
	}
}

bool GroupOrder::addToGroup(std::vector<std::vector<int>>& groupedIndexes, int current) const {
	for (auto& groupedIndex : groupedIndexes) {
		const auto isNear = std::ranges::any_of(groupedIndex, [current](int index) {
			return Game::getEnvironment()->isInLocal2Area(current, index);
		});
		if (isNear) {
			groupedIndex.push_back(current);
			return true;
		}
	}
	return false;
}

std::vector<std::vector<Unit*>> GroupOrder::divide(const std::vector<Unit*>& units) const {
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
					return Game::getEnvironment()->isInLocal2Area(value, index);
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

bool GroupOrder::expired() {
	return units.empty()
		|| toUse != nullptr && !toUse->isAlive();
}

short GroupOrder::getSize() const {
	return units.size();
}
