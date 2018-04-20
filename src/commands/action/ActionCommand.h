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
	
	virtual void addTargetAim(Vector2* to, bool append) =0;
	virtual void addFollowAim(Physical* toFollow, bool append) =0;
	virtual void addChargeAim(Vector2* charge, bool append) =0;

	OrderType action;
	Vector2* vector;
	Physical* toFollow;

	bool append;
};
