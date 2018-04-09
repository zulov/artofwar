#pragma once
#include "FormationState.h"
#include "FormationType.h"
#include "objects/Physical.h"
#include <vector>


class Formation
{
public:
	void updateIds();
	void updateSizes();
	Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Vector2 _direction);
	~Formation();

	void update();
	void changeState(FormationState newState);
	Vector2 getPositionFor(short id) const;
	float getPriority(int id) const;
	float isReady();
	FormationState getState() const { return state; };
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


	Vector2 direction;
	int leaderId;
	FormationState state;
};
