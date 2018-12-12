#include "ActionCommand.h"
#include "Game.h"
#include "UnitOrder.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/aim/ChargeAim.h"
#include "objects/unit/aim/DummyAim.h"
#include "objects/unit/aim/FollowAim.h"
#include "objects/unit/aim/TargetAim.h"
#include "simulation/env/Environment.h"
#include <chrono>


ActionCommand::ActionCommand(UnitOrder action, const Physical* toFollow, Urho3D::Vector2* vector, bool append):
	action(action), vector(vector), toUse(toFollow), append(append) {
}

ActionCommand::~ActionCommand() {
	delete vector;
}


ActionParameter ActionCommand::getTargetAim(int startInx, Urho3D::Vector2& to) {
	const auto path = Game::getEnvironment()->findPath(startInx, to);
	if (!path->empty()) {
		return ActionParameter();
	}
	return ActionParameter();
}

ActionParameter ActionCommand::getFollowAim(int startInx, Urho3D::Vector2& toSoFar, const Physical* toFollow) {
	auto const target = getTargetAim(startInx, toSoFar);

	return ActionParameter();
}

ActionParameter ActionCommand::getChargeAim(Urho3D::Vector2* charge) {
	return ActionParameter();
}

void ActionCommand::execute() {


	switch (action) {
	case UnitOrder::GO:
		return addTargetAim(vector, append);
	case UnitOrder::FOLLOW:
		if (toUse && toUse->isAlive()) {
			addFollowAim(toUse, append);
		}
		break;
	case UnitOrder::CHARGE:
		return addChargeAim(vector, append);
	case UnitOrder::ATTACK:
		return addAttackAim(toUse, append);
	case UnitOrder::DEAD:
		return addDeadAim();
	case UnitOrder::DEFEND:
		return addDefendAim();
	}
}
