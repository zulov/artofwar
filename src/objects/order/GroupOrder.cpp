#include "GroupOrder.h"
#include "FormationOrder.h"
#include "Game.h"
#include "math/VectorUtils.h"
#include "objects/unit/ActionParameter.h"
#include "objects/order/enums/UnitAction.h"
#include "objects/order/enums/UnitActionType.h"
#include "simulation/formation/FormationManager.h"


GroupOrder::GroupOrder(std::vector<Physical*>* entities, UnitActionType actionType, short id,
                       Urho3D::Vector2& vector, bool append):
	UnitOrder(actionType, id, append, vector) {
	for (auto unit : *entities) {
		//TODO performance spróbowaæ z insertem
		this->units.emplace_back(reinterpret_cast<Unit*>(unit));
	}
}

GroupOrder::GroupOrder(std::vector<Physical*>* entities, UnitActionType actionType, short id,
                       Physical* toUse, bool append):
	UnitOrder(actionType, id, append, toUse) {
	for (auto unit : *entities) {
		//TODO performance spróbowaæ z insertem
		this->units.emplace_back(reinterpret_cast<Unit*>(unit));
	}
}

GroupOrder::~GroupOrder() = default;

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

void GroupOrder::clean() {
	cleanDead(units);
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
			opt.value()->addOrder(
				new FormationOrder(opt.value(), UnitActionType::FORMATION, id, *vector, append));
		} else {
			opt.value()->addOrder(
				new FormationOrder(opt.value(), UnitActionType::FORMATION, id, toUse, append));
		}

	}
}

bool GroupOrder::expired() {
	return units.empty();
}
