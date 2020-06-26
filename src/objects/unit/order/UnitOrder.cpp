#include "UnitOrder.h"
#include "Game.h"
#include "enums/UnitAction.h"
#include "objects/unit/aim/FollowAim.h"
#include "objects/unit/aim/TargetAim.h"
#include "simulation/env/Environment.h"
#include "utils/consts.h"

UnitOrder::UnitOrder(short id, bool append, Urho3D::Vector2& vector):
	id(id), append(append), toUse(nullptr), vector(new Urho3D::Vector2(vector)) {}

UnitOrder::UnitOrder(short id, bool append, Physical* toUse):
	id(id), append(append), toUse(toUse), vector(nullptr) {}

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
	if(expired()) {
		int a =5;
	}
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

short UnitOrder::getToUseId() const {
	assert(toUse!=nullptr);
	if (toUse) {
		return toUse->getId();
	}
}
