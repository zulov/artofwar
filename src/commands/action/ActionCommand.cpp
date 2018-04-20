#include "ActionCommand.h"
#include "Game.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/aim/ChargeAim.h"
#include "objects/unit/aim/DummyAim.h"
#include "objects/unit/aim/FollowAim.h"
#include "objects/unit/aim/TargetAim.h"
#include "simulation/env/Enviroment.h"
#include <chrono>


ActionCommand::ActionCommand(OrderType action, Physical* physical, Vector2* vector, bool append) {
	this->action = action;
	this->toFollow = physical;
	this->vector = vector;
	this->append = append;
}

ActionCommand::~ActionCommand() {
	delete vector;
}


ActionParameter ActionCommand::getTargetAim(int startInx, Vector2& to, bool append) {
	auto path = Game::get()->getEnviroment()->findPath(startInx, to);
	Aim* aim;
	if (!path->empty()) {
		aim = new TargetAim(*path);
	} else {
		aim = new DummyAim();
	}

	ActionParameter parameter(aim, append);
	return parameter;
}

ActionParameter ActionCommand::getFollowAim(Physical* toFollow, bool append) {
	ActionParameter parameter(new FollowAim(toFollow), append);
	return parameter;
}

ActionParameter ActionCommand::getChargeAim(Vector2* charge, bool append) {
	ActionParameter parameter(new ChargeAim(charge), append);
	return parameter;
}

void ActionCommand::calculateCenter(Vector2& center) {
	for (Physical* physical : *entities) {
		center.x_ += physical->getPosition()->x_;
		center.y_ += physical->getPosition()->z_;
	}
	center /= entities->size();
}

void ActionCommand::addTargetAim(Vector2* to, bool append) {
	auto start = std::chrono::system_clock::now();
	short id = static_cast<short>(action);
	if (entity) {
		ActionParameter parameter = getTargetAim(entity->getBucketIndex(-1), *to, append);
		entity->action(id, parameter);
		static_cast<Unit*>(entity)->resetFormation();
	} else if (entities) {
		auto opt = Game::get()->getFormationManager()->createFormation(entities);
		if (opt.has_value()) {
			opt.value()->setFutureTarget(*to, action);
		}
	} else {
		auto opt = formation->getLeader();
		if (opt.has_value()) {
			auto physical = opt.value();

			ActionParameter parameter = getTargetAim(physical->getBucketIndex(-1), *to, append);
			physical->action(id, parameter);
		}
	}
	Game::get()->getEnviroment()->invalidateCache();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);
	std::cout << "sum " << duration.count() << std::endl;
}

void ActionCommand::addFollowAim(Physical* toFollow, bool append) {
	short id = static_cast<short>(action);
	if (entity) {
		ActionParameter parameter = getFollowAim(toFollow, append);
		entity->action(id, parameter);
	} else {
		for (Physical* physical : *entities) {
			ActionParameter parameter = getFollowAim(toFollow, append);
			physical->action(id, parameter);
		}
	}
}

void ActionCommand::addChargeAim(Vector2* charge, bool append) {
	short id = static_cast<short>(action);
	if (entity) {
		ActionParameter parameter = getChargeAim(charge, append);
		entity->action(id, parameter);
	} else {
		for (Physical* physical : *entities) {
			ActionParameter parameter = getChargeAim(charge, append);
			physical->action(id, parameter);
		}
	}
}

void ActionCommand::appendAim() {
}

void ActionCommand::execute() {
	switch (action) {
	case OrderType::GO:
		addTargetAim(vector, append);
		break;
	case OrderType::PATROL:
		addTargetAim(vector, append);
		break;
	case OrderType::FOLLOW:
		{
		if (toFollow != nullptr && toFollow->isAlive()) {
			addFollowAim(toFollow, append);
		}
		}
		break;
	case OrderType::CHARGE:
		{
		addChargeAim(vector, append);
		}
		break;
	default: ;
	}
}
