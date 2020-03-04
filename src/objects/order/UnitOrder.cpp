#include "UnitOrder.h"
#include "Game.h"
#include "consts.h"
#include "enums/UnitAction.h"
#include "objects/unit/aim/TargetAim.h"
#include "objects/unit/aim/FollowAim.h"
#include "simulation/env/Environment.h"

UnitOrder::UnitOrder(UnitActionType actionType, short id, bool append, Physical* toUse,
                     Urho3D::Vector2* vector):
	FutureOrder(actionType, static_cast<short>(id), append),
	toUse(toUse), vector(vector) {
	if (id == 0 && vector == nullptr) {
		int a = 5;
	}
}

UnitOrder::~UnitOrder() {
	delete vector;
}

ActionParameter UnitOrder::getTargetAim(int startInx, Urho3D::Vector2& to) {
	const auto path = Game::getEnvironment()->findPath(startInx, to);
	if (!path->empty()) {
		return ActionParameter::Builder().setAim(new TargetAim(*path)).build();
	}
	return Consts::EMPTY_ACTION_PARAMETER;
}

ActionParameter UnitOrder::getFollowAim(int startInx, Urho3D::Vector2& toSoFar, Physical* toFollow) {
	auto const target = getTargetAim(startInx, toSoFar);
	//jesli jest nulem to co?
	return ActionParameter::Builder().setAim(new FollowAim(toFollow, dynamic_cast<TargetAim*>(target.aim))).build();
}

ActionParameter UnitOrder::getChargeAim(Urho3D::Vector2& charge) {
	return Consts::EMPTY_ACTION_PARAMETER;
}

void UnitOrder::execute() {
	switch (static_cast<UnitAction>(id)) {
	case UnitAction::GO:
		return addTargetAim();
	case UnitAction::FOLLOW:
		if (toUse && toUse->isAlive()) {
			addFollowAim();
		}
		break;
	case UnitAction::CHARGE:
		return addChargeAim();
	case UnitAction::ATTACK:
		return addAttackAim();
	case UnitAction::DEAD:
		return addDeadAim();
	case UnitAction::DEFEND:
		return addDefendAim();
	case UnitAction::COLLECT:
		return addCollectAim();
	case UnitAction::STOP:
		return addStopAim();
	}
}
