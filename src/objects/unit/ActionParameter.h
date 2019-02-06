#pragma once

#include  "objects/ActionType.h"

class Aim;

struct ActionParameter
{
	explicit ActionParameter(Aim* aim = nullptr)
		: aim(aim) {
	}

	explicit ActionParameter(ActionType type) : type(type) {
	}

	explicit ActionParameter(int index) : index(index) {
	}

	explicit ActionParameter(std::vector<Physical*>& _thingsToInteract) {
		thingsToInteract.insert(thingsToInteract.begin(), _thingsToInteract.begin(), _thingsToInteract.end());
	}

	Aim* aim;
	ActionType type = ActionType::NONE;
	int index = -1;
	std::vector<Physical*> thingsToInteract;
};
