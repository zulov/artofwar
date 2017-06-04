#include "ActionCommand.h"


ActionCommand::ActionCommand(std::vector<Entity*>* entities, ActionType action, Vector3* parameter) {
	this->entities = entities;
	this->action = action;
	this->aimPosition = parameter;
	this->entity = nullptr;
}

ActionCommand::ActionCommand(Entity* entity, ActionType action, Vector3* paremater) {
	this->entity = entity;
	this->action = action;
	this->aimPosition = paremater;
	this->entities = nullptr;
}

ActionCommand::~ActionCommand() {
	//delete parameter;
}

void ActionCommand::execute() {
	Aims* aims;
	if (action == ADD_AIM) {
		aims = aimContainer->getNext();
	} else if (action == APPEND_AIM) {
		aims = aimContainer->getCurrent();
	} else if(action == FOLLOW) {
		ActionParameter* localParameter = new ActionParameter();
		localParameter->setAimPosition(aimPosition);
		entity->action(action, localParameter);
		delete localParameter;
		return;
	}
	aims->add(aimPosition);
	ActionParameter* localParameter = new ActionParameter();
	localParameter->setAims(aims);
	if (entity) {
		entity->action(action, localParameter);
	} else {
		for (int i = 0; i < entities->size(); ++i) {
			(*entities)[i]->action(action, localParameter);
		}
	}
	delete localParameter;
}

void ActionCommand::setAimConteiner(AimContainer* _aimContainer) {
	aimContainer = _aimContainer;
}
