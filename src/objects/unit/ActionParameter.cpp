#include "ActionParameter.h"

#include "aim/Aim.h"
#include "objects/Physical.h"
#include "utils/consts.h"

bool ActionParameter::isFirstThingAlive() const {
	return thingToInteract != nullptr
		&& thingToInteract->isUsable();
}

void ActionParameter::reset(const ActionParameter& actionParameter) {
	delete aim;
	this->aim = actionParameter.aim;
	this->thingToInteract = actionParameter.thingToInteract;
	this->index = actionParameter.index;
}

void ActionParameter::resetUsed() {
	this->aim = nullptr;
	this->thingToInteract = nullptr;
	this->index = -1;
}
