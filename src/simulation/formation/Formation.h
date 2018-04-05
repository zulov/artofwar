#pragma once
#include "FormationType.h"
#include "objects/Physical.h"
#include <vector>
#include "FormationState.h"


class Formation
{
public:
	Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Vector2 _direction);
	~Formation();

	bool update();
	Vector2 getPositionFor(short id) const;
	float getWellFormed() const;
	float isReady();
private:
	void updateUnits();
	void updateCenter();

	short id;
	short sideA;
	short sideB;
	float sparsity = 1;
	std::vector<Unit*> units;
	FormationType type;
	Vector2 center;
	float sizeA;
	float sizeB;
	float notWellformed = 0;

	Vector2 direction;
	int leaderId;
	FormationState state;
};
