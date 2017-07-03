#pragma once
#include "State.h"


class FollowState: public State 
{
public:
	FollowState(Unit* _unit);
	~FollowState();
	void onStart() override;
	void onEnd() override;
	void execute() override;
};

