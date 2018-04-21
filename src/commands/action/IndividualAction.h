#pragma once
#include "ActionCommand.h"

class IndividualAction : public ActionCommand
{
public:
	IndividualAction(Physical* entity, OrderType action, Physical* paremeter, bool append = false);
	~IndividualAction();
private:
	void addTargetAim(Vector2* to, bool append) override;
	void addChargeAim(Vector2* charge, bool append) override;
	void addFollowAim(Physical* toFollow, bool append) override;
	Physical* entity;
};
