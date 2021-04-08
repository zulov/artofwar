#include "UnitOrder.h"
#include "Game.h"
#include "enums/UnitAction.h"
#include "objects/Physical.h"
#include "objects/unit/aim/FollowAim.h"
#include "objects/unit/aim/TargetAim.h"
#include "simulation/env/Environment.h"
#include "utils/consts.h"

UnitOrder::UnitOrder(short id, bool append, Urho3D::Vector2& vector):
	id(id), append(append), toUse(nullptr), vector(new Urho3D::Vector2(vector)) {
}

UnitOrder::UnitOrder(short id, bool append, Physical* toUse):
	id(id), append(append), toUse(toUse), vector(nullptr) {
}

UnitOrder::~UnitOrder() {
	delete vector;
}

ActionParameter UnitOrder::getTargetAim(int startInx, Urho3D::Vector2& to) {
	const auto target = getTargetAimPtr(startInx, to);
	if (target) {
		return ActionParameter(target);
	}
	return Consts::EMPTY_ACTION_PARAMETER;
}

TargetAim* UnitOrder::getTargetAimPtr(int startInx, const Urho3D::Vector2& to) const {
	const auto path = Game::getEnvironment()->findPath(startInx, to);
	if (!path->empty()) {
		return new TargetAim(*path);
	}
	return nullptr;
}


TargetAim* UnitOrder::getTargetAimPtr(int startInx, const std::vector<int>& endIdxs) const {
	const auto path = Game::getEnvironment()->findPath(startInx, endIdxs);
	if (!path->empty()) {
		return new TargetAim(*path);
	}
	return nullptr;
}

ActionParameter UnitOrder::getFollowAim(int startInx, const std::vector<int>& endIdxs) {
	auto const target = getTargetAimPtr(startInx, endIdxs);
	if (target) {
		return ActionParameter(new FollowAim(toUse, target));
	}
	return Consts::EMPTY_ACTION_PARAMETER;
	//assert(target != nullptr); //TODO check
}

ActionParameter UnitOrder::getChargeAim(Urho3D::Vector2& charge) {
	return Consts::EMPTY_ACTION_PARAMETER;
}

void UnitOrder::execute() {
	if (expired()) {
		assert(false);
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
