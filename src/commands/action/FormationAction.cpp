#include "FormationAction.h"
#include "Game.h"
#include <chrono>
#include <iostream>
#include "simulation/env/Enviroment.h"


FormationAction::FormationAction(Formation* formation, OrderType action, Vector2* parameter, bool append)
	: ActionCommand(action, nullptr, parameter, append) {
	this->formation = formation;
}

FormationAction::~FormationAction() {
}

void FormationAction::addTargetAim(Vector2* to, bool append) {
	auto start = std::chrono::system_clock::now();
	short id = static_cast<short>(action);

	auto opt = formation->getLeader();
	if (opt.has_value()) {
		auto physical = opt.value();

		ActionParameter parameter = getTargetAim(physical->getBucketIndex(-1), *to, append);
		physical->action(id, parameter);
	}

	Game::get()->getEnviroment()->invalidateCache();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);
	std::cout << "sum " << duration.count() << std::endl;
}

void FormationAction::addChargeAim(Vector2* charge, bool append) {
}

void FormationAction::addFollowAim(Physical* toFollow, bool append) {
	
}
