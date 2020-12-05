#include "ActionParameter.h"

#include "objects/Physical.h"

bool ActionParameter::isFirstThingAlive() const {
	return thingToInteract != nullptr
		&& thingToInteract->isUsable();
}
