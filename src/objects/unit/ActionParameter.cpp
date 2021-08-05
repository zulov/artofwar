#include "ActionParameter.h"

#include "aim/Aim.h"
#include "objects/Physical.h"

bool ActionParameter::isFirstThingAlive() const {
	return thingToInteract != nullptr
		&& thingToInteract->isUsable();
}

void ActionParameter::reset(const ActionParameter& actionParameter) {
	delete aim;
	this->aim = actionParameter.aim;
	this->thingToInteract = actionParameter.thingToInteract;
}

void ActionParameter::resetUsed() {
	this->aim = nullptr;
	this->thingToInteract = nullptr;
}
