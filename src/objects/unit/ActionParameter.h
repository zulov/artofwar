#pragma once

#include  "objects/MenuAction.h"

class Aim;

struct ActionParameter
{
	explicit ActionParameter(Aim* aim = nullptr)
		: aim(aim) {
	}

	explicit ActionParameter(MenuAction type) : type(type) {
	}

	explicit ActionParameter(int index) : index(index) {
	}

	Aim* aim;
	MenuAction type = MenuAction::NONE;
	int index = -1;
};
