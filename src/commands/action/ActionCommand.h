#pragma once
#include "OrderType.h"
#include "commands/AbstractCommand.h"
#include "objects/Physical.h"


class ActionCommand : public AbstractCommand
{
public:
	ActionCommand(OrderType action, Physical* physical, Vector2* vector, bool append);
	virtual ~ActionCommand();

	void execute() override;
protected:
	ActionParameter getTargetAim(int startIdx, Vector2& to, bool append);
	ActionParameter getFollowAim(Physical* toFollow, bool append);
	ActionParameter getChargeAim(Vector2* charge, bool append);
	void addTargetAim(Vector2* to, bool append);
	void addFollowAim(Physical* toFollow, bool append);
	void addChargeAim(Vector2* charge, bool append);
	void calculateCenter(Vector2& center);
	void appendAim();

	OrderType action;
	Vector2* vector;
	Physical* toFollow;

	bool append;
};
