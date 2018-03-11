#pragma once
#include "Formation.h"
#include <vector>


class FormationManager
{
public:
	FormationManager();
	~FormationManager();
	void createFormation(std::vector<Physical*> *_units, FormationType _type);
	void update();
private:
	std::vector<Formation*> formations; 
};

