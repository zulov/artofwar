#pragma once
#include "FormationType.h"
#include <vector>
#include "objects/Physical.h"

class Formation
{
public:
	Formation(short _id, std::vector<Physical*>* _units, FormationType _type);
	~Formation();

	void update();
	Vector3 getPositionFor(short id);
private:
	void updateUnits();
	void updateCenter();

	short id;
	short sideA;
	short sideB;
	float sparsity = 1;
	std::vector<Unit*> units;
	FormationType type;
	Vector3 center;
};
