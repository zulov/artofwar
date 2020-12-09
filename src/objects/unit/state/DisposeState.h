#pragma once
#include "State.h"

class DisposeState : public State
{
public:
	DisposeState(): State({}) {
	}

	~DisposeState() = default;
};
