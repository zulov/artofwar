#pragma once
#include "Formation.h"
#include <vector>

#include "objects/unit/Unit.h"


class FormationManager {
public:
	FormationManager() = default;
	~FormationManager();
	std::optional<Formation*> createFormation(const std::vector<Unit*>& _units,
	                                          FormationType _type = FormationType::SQUERE);
	void update();
	float getPriority(Unit* unit);
	bool isLeader(Unit* unit) const;
	bool isMoving(Unit* unit) const;
	Formation* getFormation(Unit* unit) const { return formations[unit->getFormation()]; }
private:
	std::vector<Formation*> formations;
	unsigned currentlyFree = 0;
};
