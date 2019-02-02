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

	Aim* aim;
	ActionType type = ActionType::NONE;
	int index = -1;
};
