#pragma once
#include "ActionCommand.h"
#include "simulation/formation/Formation.h"

class FormationAction : public ActionCommand
{
public:

	FormationAction(Formation* formation, OrderType action, Vector2* parameter, bool append = false);
	~FormationAction();
private:
	Formation* formation;
};
