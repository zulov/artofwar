#pragma once

#include "aim/Aim.h"

enum class MenuAction : char;

struct ActionParameter
{
	explicit ActionParameter(Aim* aim = nullptr)
		: aim(aim) {
	}

	explicit ActionParameter(MenuAction _type) : type(_type) {
	}

	Aim* aim;
	MenuAction type;
};
