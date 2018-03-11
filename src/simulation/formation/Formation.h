#pragma once
#include "FormationType.h"
#include "objects/Physical.h"
#include <vector>


class Formation
{
public:
	Formation(short _id, std::vector<Physical*>* _units, FormationType _type);
	~Formation();

	bool update();
	Vector3 getPositionFor(short id) const;
private:
	void updateUnits();
	void updateCenter();

	short id;
	short sideA;
	short sideB;
	float sparsity = 2;
	std::vector<Unit*> units;
	FormationType type;
	Vector3 center;
	float sizeA;
	float sizeB;
};
