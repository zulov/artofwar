#include "ActionCommand.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/aim/ChargeAim.h"
#include "objects/unit/aim/FollowAim.h"
#include "objects/unit/aim/TargetAim.h"
#include "Game.h"
#include "simulation/env/Enviroment.h"


ActionCommand::ActionCommand(std::vector<Physical*>* entities, OrderType action, Vector3* parameter) {
	this->entities = entities;
	this->action = action;
	this->vector = parameter;
	this->entity = nullptr;
	this->toFollow = nullptr;
}

ActionCommand::ActionCommand(std::vector<Physical*>* entities, OrderType action, Physical* paremeter) {
	this->entities = entities;
	this->action = action;
	this->toFollow = paremeter;
	this->entity = nullptr;
	this->vector = nullptr;
}

ActionCommand::ActionCommand(Physical* entity, OrderType action, Physical* paremeter) {
	this->entity = entity;
	this->action = action;
	this->toFollow = paremeter;
	this->entities = nullptr;
	this->vector = nullptr;
}

ActionCommand::~ActionCommand() {
	delete vector;
}


ActionParameter ActionCommand::getTargetAim(Physical * physical) {
	ActionParameter parameter;
	std::vector<int> path = Game::get()->getEnviroment()->findPath(physical->getBucketIndex(-1), *vector);
	parameter.aim = new TargetAim(path);
	return parameter;
}

ActionParameter ActionCommand::getFollowAim(Physical* toFollow) {
	ActionParameter parameter;
	parameter.aim = new FollowAim(toFollow);
	return parameter;
}

ActionParameter ActionCommand::getChargeAim(Vector3* charge) {
	ActionParameter parameter;
	parameter.aim = new ChargeAim(vector);
	return parameter;
}

void ActionCommand::addTargetAim() {
	short id = static_cast<short>(action);
	if (entity) {
		ActionParameter parameter = getTargetAim(entity);
		entity->action(id, parameter);
	} else {
		for (Physical* physical : (*entities)) {
			ActionParameter parameter = getTargetAim(physical);
			physical->action(id, parameter);
		}
	}
}

void ActionCommand::addFollowAim(Physical* toFollow) {
	short id = static_cast<short>(action);
	if (entity) {
		ActionParameter parameter = getFollowAim(toFollow);
		entity->action(id, parameter);
	} else {
		for (Physical* physical : (*entities)) {
			ActionParameter parameter = getFollowAim(toFollow);
			physical->action(id, parameter);
		}
	}
}

void ActionCommand::addChargeAim(Vector3* charge) {
	short id = static_cast<short>(action);
	if (entity) {
		ActionParameter parameter = getChargeAim(charge);
		entity->action(id, parameter);
	} else {
		for (Physical* physical : (*entities)) {
			ActionParameter parameter = getChargeAim(charge);
			physical->action(id, parameter);
		}
	}
}

void ActionCommand::appendAim() {
}

void ActionCommand::execute() {
	switch (action) {
	case OrderType::GO:
		addTargetAim();
		break;
	case OrderType::PATROL:
		appendAim(); //TODO to jest zle tu trzeba wziac z unita jego cel
		break;
	case OrderType::FOLLOW:
		{
		if (toFollow != nullptr && toFollow->isAlive()) {
			addFollowAim(toFollow);
		}
		}
		break;
	case OrderType::CHARGE:
		{
		addChargeAim(vector);
		}
		break;
	default: ;
	}
}

void ActionCommand::setAimConteiner(AimContainer* _aimContainer) {
	aimContainer = _aimContainer;
}
