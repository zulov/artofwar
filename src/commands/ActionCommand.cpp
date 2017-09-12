#include "ActionCommand.h"


ActionCommand::ActionCommand(std::vector<Physical*>* entities, OrderType action, Vector3* parameter) {
	this->entities = entities;
	this->action = action;
	this->aimPosition = parameter;
	this->entity = nullptr;
	this->toFollow = nullptr;
}

ActionCommand::ActionCommand(std::vector<Physical*>* entities, OrderType action, Physical* paremeter) {
	this->entities = entities;
	this->action = action;
	this->toFollow = paremeter;
	this->entity = nullptr;
	this->aimPosition = nullptr;
}

ActionCommand::ActionCommand(Physical* entity, OrderType action, Vector3* paremeter) {
	this->entity = entity;
	this->action = action;
	this->aimPosition = paremeter;
	this->entities = nullptr;
	this->toFollow = nullptr;
}

ActionCommand::ActionCommand(Physical* entity, OrderType action, Physical* paremeter) {
	this->entity = entity;
	this->action = action;
	this->toFollow = paremeter;
	this->entities = nullptr;
	this->aimPosition = nullptr;
}

ActionCommand::~ActionCommand() {
}

void ActionCommand::applyAim(ActionParameter* parameter) {
	short id = static_cast<int>(action);
	if (entity) {
		entity->action(id, parameter);
	} else {
		for (Physical* physical : (*entities)) {
			physical->action(id, parameter);
		}
	}
}

void ActionCommand::applyAim(Aims* aims) {
	aims->add(aimPosition);
	ActionParameter* localParameter = new ActionParameter();
	localParameter->setAims(aims);
	applyAim(localParameter);
	delete localParameter;
}

void ActionCommand::execute() {
	switch (action) {
	case OrderType::GO:
		applyAim(aimContainer->getNext());
		break;
	case OrderType::PATROL:
		applyAim(aimContainer->getCurrent());
		break;
	case OrderType::FOLLOW:
		{
		if (toFollow != nullptr && toFollow->isAlive()) {
			ActionParameter* localParameter = new ActionParameter();
			localParameter->setFollowTo(toFollow);
			applyAim(localParameter);//TODO tu leci null pointer
			delete localParameter;
		}
		}
		break;
	default: ;
	}
}

void ActionCommand::setAimConteiner(AimContainer* _aimContainer) {
	aimContainer = _aimContainer;
}
