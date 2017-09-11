#pragma once
#include "State.h"


class FollowState: public State 
{
public:
	FollowState();
	~FollowState();
	void onStart(Unit* unit) override;
	void onStart(Unit* unit, ActionParameter* parameter) override;
	void onEnd(Unit* unit) override;
	void execute(Unit* unit) override;
};

