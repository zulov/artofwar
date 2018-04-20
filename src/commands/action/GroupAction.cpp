#include "GroupAction.h"
#include "Game.h"
#include <chrono>
#include <iostream>
#include "simulation/env/Enviroment.h"

GroupAction::GroupAction(std::vector<Physical*>* entities, OrderType action, Vector2* parameter, bool append)
	: ActionCommand(action, nullptr, parameter, append) {
	this->entities = entities;

}

GroupAction::GroupAction(std::vector<Physical*>* entities, OrderType action, Physical* paremeter, bool append)
	: ActionCommand(action, paremeter, nullptr, append) {
	this->entities = entities;
}

GroupAction::~GroupAction() {
}

void GroupAction::addTargetAim(Vector2* to, bool append) {
	auto start = std::chrono::system_clock::now();

	auto opt = Game::get()->getFormationManager()->createFormation(entities);
	if (opt.has_value()) {
		opt.value()->setFutureTarget(*to, action);
	}

	Game::get()->getEnviroment()->invalidateCache();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);
	std::cout << "sum " << duration.count() << std::endl;
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
