#pragma once

#include "aim/Aim.h"
#include "objects/ActionType.h"

struct ActionParameter
{
	explicit ActionParameter(Aim* aim = nullptr, bool aimAppend = false)
		: aim(aim) {
	}

	ActionParameter(ActionType _type) : type(_type) {
	}

	Aim* aim;
	ActionType type;
};
