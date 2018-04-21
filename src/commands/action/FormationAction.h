#pragma once
#include "ActionCommand.h"
#include "simulation/formation/Formation.h"

class FormationAction : public ActionCommand
{
public:

	FormationAction(Formation* formation, OrderType action, Vector2* parameter, bool append = false);
	~FormationAction();
private:
	void addTargetAim(Vector2* to, bool append) override;
	void addChargeAim(Vector2* charge, bool append) override;
	void addFollowAim(Physical* toFollow, bool append) override;
	Formation* formation;
};
