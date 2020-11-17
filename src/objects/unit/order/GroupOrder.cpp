#include "GroupOrder.h"
#include "FormationOrder.h"
#include "Game.h"
#include "enums/UnitActionType.h"
#include "objects/unit/Unit.h"
#include "simulation/formation/FormationManager.h"


GroupOrder::GroupOrder(const std::vector<Physical*>& entities, UnitActionType actionType, short id,
                       Physical* toUse, bool append): UnitOrder(id, append, toUse), actionType(actionType) {
	addUnits(entities);
}

GroupOrder::GroupOrder(const std::vector<Physical*>& entities, UnitActionType actionType, short id,
                       Urho3D::Vector2 vector, bool append): UnitOrder(id, append, vector), actionType(actionType) {
	addUnits(entities);
}

GroupOrder::GroupOrder(const std::vector<Unit*>& entities, UnitActionType actionType, short id,
                       Urho3D::Vector2 vector, bool append): UnitOrder(id, append, vector), actionType(actionType),
                                                             units(entities) {
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
	case UnitActionType::FORMATION:
	{
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
	for (auto unit : units) {
		unit->action(static_cast<UnitAction>(id), parameter);
	}
}

void GroupOrder::simpleAction() const {
	for (auto unit : units) {
		unit->action(static_cast<UnitAction>(id));
	}
}

void GroupOrder::transformToFormationOrder() const {
	auto opt = Game::getFormationManager()->createFormation(units);
	if (opt.has_value()) {
		if (vector) {
			opt.value()->addOrder(new FormationOrder(opt.value(), id, *vector, append));
		} else {
			opt.value()->addOrder(new FormationOrder(opt.value(), id, toUse, append));
		}

	}
}

bool GroupOrder::expired() {
	return units.empty()
		|| toUse != nullptr && !toUse->isAlive();
}

short GroupOrder::getSize() const {
	return units.size();
}
