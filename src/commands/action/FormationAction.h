#pragma once
#include "ActionCommand.h"
#include "simulation/formation/Formation.h"

class FormationAction : public ActionCommand
{
public:
	FormationAction(Formation* formation, UnitOrder action, const Physical* physical, Vector2* vector, bool append = false);
	~FormationAction();
private:
	void addTargetAim(Vector2* to, bool append) override;
	void addChargeAim(Vector2* charge, bool append) override;
	void addFollowAim(const Physical* toFollow, bool append) override;
	Formation* formation;
};
