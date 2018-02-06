#include "ActionCommand.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/aim/AimContainer.h"
#include "objects/unit/aim/ChargeAim.h"
#include "objects/unit/aim/FollowAim.h"
#include "objects/unit/aim/TargetAim.h"


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


void ActionCommand::applyAction(ActionParameter& parameter) {
	short id = static_cast<short>(action);
	if (entity) {
		entity->action(id, parameter);
	} else {
		for (Physical* physical : (*entities)) {
			physical->action(id, parameter);
		}
	}
}

void ActionCommand::applyAim(Aims* aims) {
	aims->add(new TargetAim(vector));
	ActionParameter localParameter;
	localParameter.setAims(aims);
	applyAction(localParameter);
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
			Aims* aims = aimContainer->getNext();
			aims->add(new FollowAim(toFollow));
			ActionParameter localParameter;
			localParameter.setAims(aims);
			applyAction(localParameter);
		}
		}
		break;
	case OrderType::CHARGE:
		{
		Aims* aims = aimContainer->getNext();
		aims->add(new ChargeAim(vector));
		ActionParameter localParameter;
		localParameter.setAims(aims);
		applyAction(localParameter);
		}
		break;
	default: ;
	}
}

void ActionCommand::setAimConteiner(AimContainer* _aimContainer) {
	aimContainer = _aimContainer;
}
