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


ActionCommand::ActionCommand(UnitOrder action, const Physical* physical, Urho3D::Vector2* vector, bool append):
	action(action), vector(vector), toFollow(physical), append(append) {
}

ActionCommand::~ActionCommand() {
	delete vector;
}


ActionParameter ActionCommand::getTargetAim(int startInx, Urho3D::Vector2& to) {
	const auto path = Game::getEnvironment()->findPath(startInx, to);
	if (!path->empty()) {
		return ActionParameter(new TargetAim(*path));
	}
	return ActionParameter(new DummyAim());
}

ActionParameter ActionCommand::getFollowAim(int startInx, Urho3D::Vector2& toSoFar, const Physical* toFollow) {
	auto const target = getTargetAim(startInx, toSoFar);

	return ActionParameter(new FollowAim(toFollow, static_cast<TargetAim*>(target.aim)));
}

ActionParameter ActionCommand::getChargeAim(Urho3D::Vector2* charge) {
	return ActionParameter(new ChargeAim(charge));
}


void ActionCommand::execute() {
	switch (action) {
	case UnitOrder::GO:
		return addTargetAim(vector, append);
	case UnitOrder::FOLLOW:
		if (toFollow && toFollow->isAlive()) {
			addFollowAim(toFollow, append);
		}
		break;
	case UnitOrder::CHARGE:
		return addChargeAim(vector, append);
	}
}
