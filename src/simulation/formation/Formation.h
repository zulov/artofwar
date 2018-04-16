#pragma once
#include "FormationState.h"
#include "FormationType.h"
#include "OrderType.h"
#include "objects/Physical.h"
#include <vector>


class Formation
{
public:
	Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Vector2 _direction);
	~Formation();
	void electLeader();

	void update();
	Vector2 getPositionFor(short id);
	float getPriority(int id) const;
	FormationState getState() const { return state; }
	std::optional<Physical*> getLeader();
	void setFutureTarget(const Vector2& _futureTarget, OrderType _action);
	size_t getSize();
	void semiReset();
private:
	void updateUnits();
	void updateCenter();
	void updateIds();
	void updateSizes();
	void calculateNotWellFormed();

	void changeState(FormationState newState);
	short id;
	short sideA;
	short sideB;
	float sparsity = 1;

	std::vector<Unit*> units;

	FormationType type;
	Vector2 center;
	float sizeA;
	float sizeB;

	bool changed = true;

	Vector2 direction;
	int leaderId;
	FormationState state;

	Aims aims;
	Vector2 futureTarget;
	OrderType action;
	bool hasOrder = false;

	float theresholedMin = 0.1;
	float theresholedMax = 0.5;
	float notWellFormed = 1.0;
};
