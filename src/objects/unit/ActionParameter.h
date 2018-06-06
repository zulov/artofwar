#pragma once

#include "aim/Aim.h"
enum class MenuAction : char;

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
