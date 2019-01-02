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


ActionCommand::ActionCommand(FutureOrder& futureAim, bool append): futureAim(futureAim),
	append(append) {
}

ActionParameter ActionCommand::getTargetAim(int startInx, Urho3D::Vector2& to) {
	const auto path = Game::getEnvironment()->findPath(startInx, to);
	if (!path->empty()) {
		return ActionParameter(new TargetAim(*path));
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
	addAim();
}
