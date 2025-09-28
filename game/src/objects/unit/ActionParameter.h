#pragma once

class Aim;
class Physical;

struct ActionParameter {
	ActionParameter() = default;

	explicit ActionParameter(Aim* aim) : aim(aim) {}

	explicit ActionParameter(Physical* thing) : thingToInteract(thing) {}

	bool isThingAlive() const;
	void reset(const ActionParameter& actionParameter, bool withDelete);
	void reset(bool withDelete);
	void resetUsed();

	union {
		Aim* aim = nullptr;
		Physical* thingToInteract;
	};
};
