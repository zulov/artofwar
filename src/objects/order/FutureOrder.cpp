#include "FutureOrder.h"
#include "Game.h"
#include "objects/unit/aim/TargetAim.h"
#include "simulation/env/Environment.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/aim/FollowAim.h"
#include "consts.h"

FutureOrder::FutureOrder(short actionType, short id, bool append): action(actionType), id(id), append(append) {
}

FutureOrder::~FutureOrder() = default;

ActionParameter FutureOrder::getTargetAim(int startInx, Urho3D::Vector2& to) {
	const auto path = Game::getEnvironment()->findPath(startInx, to);
	if (!path->empty()) {
		return ActionParameter::Builder().setAim(new TargetAim(*path)).build();
	}
	return Consts::EMPTY_ACTION_PARAMETER;
}

ActionParameter FutureOrder::getFollowAim(int startInx, Urho3D::Vector2& toSoFar, Physical* toFollow) {
	auto const target = getTargetAim(startInx, toSoFar);
	//jesli jest nulem to co?
	return ActionParameter::Builder().setAim(new FollowAim(toFollow, static_cast<TargetAim*>(target.aim))).build();
}

ActionParameter FutureOrder::getChargeAim(Urho3D::Vector2& charge) {
	return Consts::EMPTY_ACTION_PARAMETER;
}

void FutureOrder::execute() {
	switch (action) {
	case UnitAction::GO:
		return addTargetAim();
	case UnitAction::FOLLOW:
		if (toUse &&toUse
		->
		isAlive()
		)
		{
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

