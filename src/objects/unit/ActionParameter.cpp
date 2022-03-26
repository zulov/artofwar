#include "ActionParameter.h"

#include "aim/Aim.h"
#include "objects/Physical.h"

bool ActionParameter::isThingAlive() const {
	return thingToInteract != nullptr
		&& thingToInteract->isUsable();
}

void ActionParameter::reset(const ActionParameter& actionParameter, bool withDelete) {
	if (aim && withDelete) {
		delete aim;
	}
	this->aim = actionParameter.aim;
	this->thingToInteract = actionParameter.thingToInteract;
}

void ActionParameter::reset(bool withDelete) {
	if (aim && withDelete) {
		delete aim;
	}
	resetUsed();
}

void ActionParameter::resetUsed() {
	this->aim = nullptr;
	this->thingToInteract = nullptr;
}
