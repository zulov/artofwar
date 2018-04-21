#include "GroupAction.h"
#include "Game.h"
#include "simulation/env/Enviroment.h"

GroupAction::GroupAction(std::vector<Physical*>* entities, OrderType action, Vector2* parameter, bool append)
	: ActionCommand(action, nullptr, parameter, append) {
	this->entities = entities;
}

GroupAction::GroupAction(std::vector<Physical*>* entities, OrderType action, Physical* paremeter, bool append)
	: ActionCommand(action, paremeter, nullptr, append) {
	this->entities = entities;
}

GroupAction::~GroupAction() = default;

void GroupAction::addTargetAim(Vector2* to, bool append) {
	auto opt = Game::get()->getFormationManager()->createFormation(entities);
	if (opt.has_value()) {
		opt.value()->appendFutureTarget(*to, action);
	}
}

void GroupAction::addChargeAim(Vector2* charge, bool append) {
	short id = static_cast<short>(action);

	for (Physical* physical : *entities) {
		ActionParameter parameter = getChargeAim(charge, append);
		physical->action(id, parameter);
	}
}

void GroupAction::addFollowAim(Physical* toFollow, bool append) {
	short id = static_cast<short>(action);

	for (Physical* physical : *entities) {
		ActionParameter parameter = getFollowAim(toFollow, append);
		physical->action(id, parameter);
	}
}
