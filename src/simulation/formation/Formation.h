#pragma once
#include "FormationState.h"
#include "FormationType.h"
#include "objects/Physical.h"
#include <vector>
#include "OrderType.h"


class Formation
{
public:
	Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Vector2 _direction);
	~Formation();

	void update();
	Vector2 getPositionFor(short id) const;
	float getPriority(int id) const;
	FormationState getState() const { return state; }
	Physical* getLeader();
	void setFutureTarget(const Vector2& _futureTarget, OrderType _action);
private:
	void updateUnits();
	void updateCenter();
	void updateIds();
	void updateSizes();
	void calculateNotWellFormed();

	void changeState(FormationState newState);
	short id;
	short sideA;
	float sparsity = 1;
	std::vector<Unit*> units;
	FormationType type;
	Vector2 center;
	float sizeA;
	float sizeB;


	Vector2 direction;
	int leaderId;
	FormationState state;

	Aims aims;
	Vector2 futureTarget;
	OrderType action;

	float theresholed = 0.1;
	float notWellFormed = 1.0;
};
