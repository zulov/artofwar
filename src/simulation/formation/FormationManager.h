#pragma once
#include "Formation.h"
#include <optional>
#include <vector>


class FormationManager
{
public:
	FormationManager();
	~FormationManager();
	Formation* createFormation(std::vector<Physical*>* _units, FormationType _type = FormationType::SQUERE);
	void update();
	float getPriority(Unit* unit);
	std::optional<Vector2> getPositionFor(Unit* unit);
private:
	std::vector<Formation*> formations;
	int currentlyFree = 0;
	
};
