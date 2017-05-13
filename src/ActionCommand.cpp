#include "ActionCommand.h"


ActionCommand::ActionCommand(std::vector<Entity*>* entities, ActionType action, Vector3* parameter) {
	this->entities = entities;
	this->action = action;
	this->pos = parameter;
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
	}
	aims->add(pos);
	ActionParameter* localParameter = new ActionParameter();
	localParameter->setAims(aims);

	for (int i = 0; i < entities->size(); ++i) {
		(*entities)[i]->action(action, localParameter);
	}
	delete localParameter;
}

void ActionCommand::setAimConteiner(AimContainer* _aimContainer) {
	aimContainer = _aimContainer;
}
