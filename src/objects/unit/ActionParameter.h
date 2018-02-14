#pragma once

#include "aim/Aims.h"


struct ActionParameter
{
	ActionParameter(Aim* aim = nullptr, bool aimAppend = false)
		: aim(aim),
		aimAppend(aimAppend) {
	}

	Aim* aim;
	bool aimAppend;
};
