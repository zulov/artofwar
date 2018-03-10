#pragma once
#include "objects/unit/Unit.h"
#include "Formation.h"
#include <vector>


class FormationManager
{
public:
	FormationManager();
	~FormationManager();
	void createFormation(std::vector<Unit*> *_units, FormationType _type);
	void update();
private:
	std::vector<Formation*> formations; 
};

