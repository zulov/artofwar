#include "ActionCommand.h"
#include "Game.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/aim/ChargeAim.h"
#include "objects/unit/aim/DummyAim.h"
#include "objects/unit/aim/FollowAim.h"
#include "objects/unit/aim/TargetAim.h"
#include "simulation/env/Enviroment.h"
#include <chrono>


ActionCommand::ActionCommand(std::vector<Physical*>* entities, OrderType action, Vector3* parameter, bool append) {
	this->entities = entities;
	this->action = action;
	this->vector = parameter;
	this->entity = nullptr;
	this->toFollow = nullptr;
	this->append = append;
}

ActionCommand::ActionCommand(std::vector<Physical*>* entities, OrderType action, Physical* paremeter, bool append) {
	this->entities = entities;
	this->action = action;
	this->toFollow = paremeter;
	this->entity = nullptr;
	this->vector = nullptr;
	this->append = append;
}

ActionCommand::ActionCommand(Physical* entity, OrderType action, Physical* paremeter, bool append) {
	this->entity = entity;
	this->action = action;
	this->toFollow = paremeter;
	this->entities = nullptr;
	this->vector = nullptr;
	this->append = append;
}

ActionCommand::~ActionCommand() {
	delete vector;
}


ActionParameter ActionCommand::getTargetAim(int startInx, Vector3& to, bool append) {
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

ActionParameter ActionCommand::getChargeAim(Vector3* charge, bool append) {
	ActionParameter parameter(new ChargeAim(charge), append);
	return parameter;
}

Vector3* ActionCommand::calculateCenter() {
	Vector3* center = new Vector3();
	for (Physical* physical : *entities) {
		*center += *physical->getPosition();
	}
	return center;
}

void ActionCommand::addTargetAim(Vector3* to, bool append) {
	auto start = std::chrono::system_clock::now();
	short id = static_cast<short>(action);
	if (entity) {
		ActionParameter parameter = getTargetAim(entity->getBucketIndex(-1), *to, append);
		entity->action(id, parameter);
	} else {
		Vector3* center = calculateCenter();
		*center /= entities->size();
		for (Physical* physical : *entities) {
			Vector3 pos = *physical->getPosition() - *center + *to;

			ActionParameter parameter = getTargetAim(physical->getBucketIndex(-1), pos, append);
			physical->action(id, parameter);
		}
		delete center;
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

void ActionCommand::addChargeAim(Vector3* charge, bool append) {
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
