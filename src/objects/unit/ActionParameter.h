#pragma once

#include  "objects/ActionType.h"

class Aim;

struct ActionParameter {
	struct Builder;

	ActionParameter(Aim* aim, ActionType type, int index, std::vector<Physical*>& _thingsToInteract)
		: aim(aim), type(type), index(index) {
		thingsToInteract.insert(thingsToInteract.begin(), _thingsToInteract.begin(), _thingsToInteract.end());
	}


	bool isFirstThingAlive() const {
		return !thingsToInteract.empty()
			&& thingsToInteract[0] != nullptr
			&& thingsToInteract[0]->isUsable();
	}

	Aim* aim = nullptr;
	std::vector<Physical*> thingsToInteract;
	ActionType type = ActionType::NONE;
	int index = -1;
};

struct ActionParameter::Builder {
private:
	Aim* aim = nullptr;
	ActionType type = ActionType::NONE;
	int index = -1;
	std::vector<Physical*> thingsToInteract;

public:

	Builder& setAim(Aim* aim) {
		this->aim = aim;
		return *this;
	}

	Builder& setType(ActionType type) {
		this->type = type;
		return *this;
	}

	Builder& setIndex(int index) {
		this->index = index;
		return *this;
	}

	Builder& setThingsToInteract(std::vector<Physical*>& _thingsToInteract) {
		thingsToInteract = _thingsToInteract; //TODO probable bug? czy nie lepiej kopiowac
		return *this;
	}

	Builder& setThingsToInteract(Physical* toUse) {
		thingsToInteract.push_back(toUse);
		return *this;
	}

	ActionParameter build() {
		return ActionParameter(aim, type, index, thingsToInteract);
	}
};
