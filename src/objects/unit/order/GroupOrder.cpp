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

void GroupOrder::transformToFormationOrder() const {
	std::vector<std::vector<Unit*>> groups = divide(units);
	for (auto& group : groups) {
		auto opt = Game::getFormationManager()->createFormation(group);
		if (opt.has_value()) {
			if (vector) {
				opt.value()->addOrder(new FormationOrder(opt.value(), id, *vector, append));
			} else {
				opt.value()->addOrder(new FormationOrder(opt.value(), id, toUse, append));
			}
		}
	}

	// auto opt = Game::getFormationManager()->createFormation(units);
	// if (opt.has_value()) {
	// 	if (vector) {
	// 		opt.value()->addOrder(new FormationOrder(opt.value(), id, *vector, append));
	// 	} else {
	// 		opt.value()->addOrder(new FormationOrder(opt.value(), id, toUse, append));
	// 	}
	// }
}

bool GroupOrder::addToGroup(std::vector<std::vector<int>>& groupedIndexes, int current) const {
	for (auto& groupedIndex : groupedIndexes) {
		const auto isNear = std::ranges::any_of(groupedIndex, [current](int index) {
			return Game::getEnvironment()->isInLocalArea(current, index);
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
		allIndexes.push_back(unit->getMainCell());
	}
	std::ranges::sort(allIndexes);
	allIndexes.erase(std::unique(allIndexes.begin(), allIndexes.end()), allIndexes.end());
	std::vector<std::vector<int>> groupedIndexes;

	for (int i = 0; i < allIndexes.size(); ++i) {
		auto current = allIndexes[i];

		bool added = addToGroup(groupedIndexes, current);
		if (!added) {
			std::vector<int> temp;
			temp.push_back(current);
			groupedIndexes.push_back(temp);
		}
	}
	std::vector<std::vector<Unit*>> result;
	for (auto& groupedIndex : groupedIndexes) {
		std::vector<Unit*> temp;
		//temp.reserve()
		for (auto index : groupedIndex) {
			for (auto value : units) {
				if (value->getMainCell() == index) {
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
