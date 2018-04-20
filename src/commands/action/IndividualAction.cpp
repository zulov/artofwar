#include "IndividualAction.h"
#include "Game.h"
#include <chrono>
#include <iostream>
#include "objects/unit/ActionParameter.h"
#include "simulation/env/Enviroment.h"


IndividualAction::IndividualAction(Physical* entity, OrderType action, Physical* paremeter, bool append)
	: ActionCommand(action, paremeter, nullptr, append) {
	this->entity = entity;
}

IndividualAction::~IndividualAction() {
}

void IndividualAction::addTargetAim(Vector2* to, bool append) {
	auto start = std::chrono::system_clock::now();
	short id = static_cast<short>(action);

	ActionParameter parameter = getTargetAim(entity->getBucketIndex(-1), *to, append);
	entity->action(id, parameter);
	static_cast<Unit*>(entity)->resetFormation();

	Game::get()->getEnviroment()->invalidateCache();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);
	std::cout << "sum " << duration.count() << std::endl;
}

void IndividualAction::addChargeAim(Vector2* charge, bool append) {
	short id = static_cast<short>(action);

	ActionParameter parameter = getChargeAim(charge, append);
	entity->action(id, parameter);
}

void IndividualAction::addFollowAim(Physical* toFollow, bool append) {
	short id = static_cast<short>(action);

	ActionParameter parameter = getFollowAim(toFollow, append);
	entity->action(id, parameter);
}
