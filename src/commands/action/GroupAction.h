#pragma once
#include "ActionCommand.h"

class GroupAction : public ActionCommand
{
public:
	GroupAction(std::vector<Physical*>* entities, OrderType action, Vector2* parameter, bool append = false);
	GroupAction(std::vector<Physical*>* entities, OrderType action, Physical* paremeter, bool append = false);
	~GroupAction();
private:
	void addTargetAim(Vector2* to, bool append) override;
	void addChargeAim(Vector2* charge, bool append) override;
	void addFollowAim(Physical* toFollow, bool append) override;
	std::vector<Physical*>* entities;
};
