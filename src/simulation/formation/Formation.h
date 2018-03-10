#pragma once
#include "FormationType.h"
#include "objects/unit/Unit.h"

class Formation
{
public:
	Formation(short _id, std::vector<Unit*>* _units, FormationType _type);
	~Formation();

	void update();
private:
	void updateUnits();
	void updateCenter();

	short id;
	std::vector<Unit*> units;
	FormationType type;
	Vector3 center;
};
