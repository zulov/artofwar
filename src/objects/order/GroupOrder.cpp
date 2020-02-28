#include "GroupOrder.h"
#include "FormationOrder.h"
#include "Game.h"
#include "consts.h"
#include "VectorUtils.h"
#include "objects/unit/ActionParameter.h"
#include "objects/order/enums/UnitAction.h"
#include "simulation/formation/FormationManager.h"


GroupOrder::GroupOrder(std::vector<Physical*>* entities, UnitActionType actionType, short id,
                       const Urho3D::Vector2& vector, Physical* toUse, bool append):
	UnitOrder(actionType, id, append, toUse, vector) {
	for (auto unit : *entities) {
		//TODO performance spróbowaæ z insertem
		this->units.emplace_back(reinterpret_cast<Unit*>(unit));
	}
}

GroupOrder::~GroupOrder() = default;

bool GroupOrder::add() {
	switch (UnitActionType(action)) {
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
	for (auto entity : units) {
		entity->action(static_cast<char>(FutureOrder::action), parameter);
	}
}

void GroupOrder::simpleAction() const {
	for (auto entity : units) {
		entity->action(static_cast<char>(FutureOrder::action), Consts::EMPTY_ACTION_PARAMETER);
	}
}

void GroupOrder::transformToFormationOrder() const {
	auto opt = Game::getFormationManager()->createFormation(units);
	if (opt.has_value()) {
		opt.value()->addOrder(
			new FormationOrder(opt.value(), UnitActionType::FORMATION, id, vector, toUse, append));
	}
}

bool GroupOrder::expired() {
	return units.empty();
}
