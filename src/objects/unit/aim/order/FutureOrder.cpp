#include "FutureOrder.h"
#include "Game.h"
#include "objects/unit/aim/TargetAim.h"
#include "simulation/env/Environment.h"
#include "objects/unit/ActionParameter.h"

FutureOrder::FutureOrder(const Urho3D::Vector2& vector, const Physical* toUse, UnitOrder action)
	: vector(vector),
	toUse(toUse),
	action(action) {
}

FutureOrder::FutureOrder(const Urho3D::Vector2& vector, UnitOrder action)
	: vector(vector), action(action) {
}

FutureOrder::FutureOrder(const Physical* toUse, UnitOrder action)
	: toUse(toUse), action(action) {
}

bool FutureOrder::expired() const {
	return toUse != nullptr && !toUse->isAlive();
}

void FutureOrder::execute() {
	switch (action) {
	case UnitOrder::GO:
		return addTargetAim(vector);
	case UnitOrder::FOLLOW:
		if (toUse&& toUse->isAlive()){
			addFollowAim(toUse);
		}
		break;
	case UnitOrder::CHARGE:
		return addChargeAim(vector);
	case UnitOrder::ATTACK:
		return addAttackAim(toUse);
	case UnitOrder::DEAD:
		return addDeadAim();
	case UnitOrder::DEFEND:
		return addDefendAim();
	}
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

	return ActionParameter();
}

ActionParameter FutureOrder::getChargeAim(Urho3D::Vector2* charge) {
	return ActionParameter();
}
