#pragma once

#include "aim/Aims.h"
#include "objects/queue/QueueType.h"


struct ActionParameter
{
	ActionParameter(Aim* aim = nullptr, bool aimAppend = false)
		: aim(aim),
		aimAppend(aimAppend) {
	}

	ActionParameter(QueueType _type) : type(_type) {
	}

	Aim* aim;
	bool aimAppend;
	QueueType type;
};
