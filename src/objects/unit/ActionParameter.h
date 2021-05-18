#pragma once

class Aim;
class Physical;

struct ActionParameter {
	ActionParameter() = default;

	explicit ActionParameter(Aim* aim) : aim(aim) {}

	ActionParameter(Physical* thing, int index) : index(index), thingToInteract(thing) { }

	bool isFirstThingAlive() const;
	void reset(const ActionParameter& actionParameter);
	void resetUsed();

	//Aim* aim = nullptr;//TODO union!!!!
	//Physical* thingToInteract = nullptr;

	union {
		Aim* aim = nullptr;
		Physical* thingToInteract;
	};
	int index = -1;
};
