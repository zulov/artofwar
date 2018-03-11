#pragma once

#include "aim/Aim.h"
#include "objects/ActionType.h"

struct ActionParameter
{
	ActionParameter(Aim* aim = nullptr, bool aimAppend = false)
		: aim(aim),
		aimAppend(aimAppend) {
	}

	ActionParameter(ActionType _type) : type(_type) {
	}

	Aim* aim;
	bool aimAppend;
	ActionType type;

};
