#include "ActionCommand.h"
#include "Game.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/aim/ChargeAim.h"
#include "objects/unit/aim/DummyAim.h"
#include "objects/unit/aim/FollowAim.h"
#include "objects/unit/aim/TargetAim.h"
#include "simulation/env/Enviroment.h"
#include <chrono>


ActionCommand::ActionCommand(UnitOrder action, const Physical* physical, Vector2* vector, bool append) {
	this->action = action;
	this->toFollow = physical;
	this->vector = vector;
	this->append = append;
}

ActionCommand::~ActionCommand() {
	delete vector;
}


ActionParameter ActionCommand::getTargetAim(int startInx, Vector2& to, bool append) {
	auto path = Game::getEnviroment()->findPath(startInx, to);
	if (!path->empty()) {
		return ActionParameter(new TargetAim(*path));
	}
	return ActionParameter(new DummyAim());
}

ActionParameter ActionCommand::getFollowAim(const Physical* toFollow, bool append) {
	return ActionParameter(new FollowAim(toFollow));
}

ActionParameter ActionCommand::getChargeAim(Vector2* charge, bool append) {
	return ActionParameter(new ChargeAim(charge));
}


void ActionCommand::execute() {
	switch (action) {
	case UnitOrder::GO:
		addTargetAim(vector, append);
		break;
	case UnitOrder::FOLLOW:
		{
		if (toFollow != nullptr && toFollow->isAlive()) {
			addFollowAim(toFollow, append);
		}
		}
		break;
	case UnitOrder::CHARGE:
		addChargeAim(vector, append);
		break;
	default: ;
	}
}
