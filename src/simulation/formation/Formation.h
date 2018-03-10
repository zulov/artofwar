#pragma once
#include "FormationType.h"
#include "objects/unit/Unit.h"

class Formation
{
public:
	Formation(std::vector<Unit*>* _units, FormationType _type);
	~Formation();
private:
	std::vector<Unit*> units;
	FormationType type;
};
