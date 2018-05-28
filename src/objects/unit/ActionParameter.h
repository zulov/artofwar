#pragma once

#include "aim/Aim.h"
#include "objects/MenuAction.h"

struct ActionParameter
{
	explicit ActionParameter(Aim* aim = nullptr, bool aimAppend = false)
		: aim(aim) {
	}

	ActionParameter(MenuAction _type) : type(_type) {
	}

	Aim* aim;
	MenuAction type;
};
