#pragma once
#include "FormationState.h"
#include "FormationType.h"
#include "OrderType.h"
#include "objects/Physical.h"
#include <vector>


struct FutureAim;

class Formation
{
public:
	Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Vector2& _direction);
	~Formation();

	void update();
	Vector2 getPositionFor(short id);
	float getPriority(int id) const;
	FormationState getState() const { return state; }
	std::optional<Physical*> getLeader();

	void addFutureTarget(const Vector2& _futureTarget, const Physical* _physical, OrderType _action, bool append);
	size_t getSize();
	void semiReset();
private:
	void updateUnits();
	void updateCenter();
	void updateIds();
	void updateSizes();
	void calculateNotWellFormed();
	void innerUpdate();
	void electLeader();

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
	Unit* leader = nullptr;
	Unit* oldLeader = nullptr;
	FormationState state;

	//Aims aims;
	std::vector<FutureAim> futureOrders;
	//bool hasFutureOrder = false;

	float theresholedMin = 0.1;
	float theresholedMax = 0.5;
	float notWellFormed = 1.0;
	float notWellFormedExact = 1.0;
};
