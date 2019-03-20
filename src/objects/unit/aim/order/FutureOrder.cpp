#include "FutureOrder.h"
#include "Game.h"
#include "objects/unit/aim/TargetAim.h"
#include "simulation/env/Environment.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/aim/FollowAim.h"
#include "consts.h"

FutureOrder::FutureOrder(UnitOrder action, bool append, const Urho3D::Vector2& vector, Physical* toUse)
	: vector(vector), toUse(toUse), action(action), append(append) {
}

FutureOrder::FutureOrder(UnitOrder action, bool append, const Urho3D::Vector2& vector)
	: vector(vector), toUse(nullptr), action(action), append(append) {
}

FutureOrder::FutureOrder(UnitOrder action, bool append, Physical* toUse)
	: toUse(toUse), action(action), append(append) {
}

FutureOrder::~FutureOrder() = default;

bool FutureOrder::expired() const {
	return toUse != nullptr && !toUse->isAlive();
}

ActionParameter FutureOrder::getTargetAim(int startInx, Urho3D::Vector2& to) {
	const auto path = Game::getEnvironment()->findPath(startInx, to);
	if (!path->empty()) {
		return ActionParameter::Builder().setAim(new TargetAim(*path)).build();
	}
	return Consts::EMPTY_ACTION_PARAMETER;
}

ActionParameter FutureOrder::getFollowAim(int startInx, Urho3D::Vector2& toSoFar, const Physical* toFollow) {
	auto const target = getTargetAim(startInx, toSoFar);
	//jesli jest nulem to co?
	return ActionParameter::Builder().setAim(new FollowAim(toFollow, static_cast<TargetAim*>(target.aim))).build();
}

ActionParameter FutureOrder::getChargeAim(Urho3D::Vector2& charge) {
	return Consts::EMPTY_ACTION_PARAMETER;
}

void FutureOrder::execute() {
	switch (action) {
	case UnitOrder::GO:
		return addTargetAim();
	case UnitOrder::FOLLOW:
		if (toUse && toUse->isAlive()) {
			addFollowAim();
		}
		break;
	case UnitOrder::CHARGE:
		return addChargeAim();
	case UnitOrder::ATTACK:
		return addAttackAim();
	case UnitOrder::DEAD:
		return addDeadAim();
	case UnitOrder::DEFEND:
		return addDefendAim();
	case UnitOrder::COLLECT:
		return addCollectAim();
	case UnitOrder::STOP:
		return addStopAim();
	}
}
