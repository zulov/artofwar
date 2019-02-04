#include "FutureOrder.h"
#include "Game.h"
#include "objects/unit/aim/TargetAim.h"
#include "simulation/env/Environment.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/aim/FollowAim.h"

FutureOrder::FutureOrder(UnitOrder action, bool append, const Urho3D::Vector2& vector, const Physical* toUse)
	: vector(vector), toUse(toUse), action(action), append(append) {
}

FutureOrder::FutureOrder(UnitOrder action, bool append, const Urho3D::Vector2& vector)
	: vector(vector), toUse(nullptr), action(action), append(append) {
}

FutureOrder::FutureOrder(UnitOrder action, bool append, const Physical* toUse)
	: toUse(toUse), action(action), append(append) {
}

FutureOrder::~FutureOrder() = default;

bool FutureOrder::expired() const {
	return toUse != nullptr && !toUse->isAlive();
}

ActionParameter FutureOrder::getTargetAim(int startInx, Urho3D::Vector2& to) {
	const auto path = Game::getEnvironment()->findPath(startInx, to);
	if (!path->empty()) {
		return ActionParameter(new TargetAim(*path));
	}
	return ActionParameter();
}

ActionParameter FutureOrder::getFollowAim(int startInx, Urho3D::Vector2& toSoFar, const Physical* toFollow) {
	auto const target = getTargetAim(startInx, toSoFar);
	//jesli jest nulem to co?
	return ActionParameter(new FollowAim(toFollow, static_cast<TargetAim*>(target.aim)));
}

ActionParameter FutureOrder::getChargeAim(Urho3D::Vector2& charge) {
	return ActionParameter();
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
	}
}
