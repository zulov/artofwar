#pragma once

class Aim;
class Physical;

struct ActionParameter {
	ActionParameter() = default;

	explicit ActionParameter(Aim* aim) : aim(aim) {}

	ActionParameter(Physical* thing, int index) : index(index), thingToInteract(thing) { }

	bool isFirstThingAlive() const;

	Aim* aim = nullptr;
	Physical* thingToInteract;
	int index = -1;
};
