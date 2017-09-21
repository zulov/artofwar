#pragma once

#include <vector>
#include "ObjectEnums.h"
#include "defines.h"
#include "Game.h"
#include "objects/Entity.h"
#include "state/UnitStateType.h"
#include "UnitType.h"
#include "objects/Physical.h"
#include "state/StateManager.h"

class State;

class Unit : public Physical
{
	friend class State;
	friend class StopState;
	friend class DefendState;
	friend class DeadState;
	friend class GoState;
	friend class PatrolState;
	friend class FollowState;
	friend class AttackState;
	friend class ChargeState;
public:
	Unit(Vector3* _position, Urho3D::Node* _boxNode);
	~Unit();
	double getHealthBarSize() override;
	void populate(db_unit* _dbUnit, StateManager* _states);
	void checkAim();
	void move(double timeStep);
	void applyForce(double timeStep);
	int getSubTypeId() override;
	void setAcceleration(Vector3* _acceleration);
	double getMaxSeparationDistance();
	Vector3* getDestination();
	Vector3* getVelocity();
	double getMass();
	double getUnitRadius();
	void absorbAttack(double attackCoef) override;

	void attack(vector<Physical*>* enemies);
	void attack(Physical* enemy);
	void attack();
	void updateHeight(double y, double timeStep);
	String* toMultiLineString() override;
	void action(short id, ActionParameter* parameter) override;
	UnitStateType getState();
	void clean() override;
	void setState(UnitStateType state);
	bool checkTransition(UnitStateType state);
	void executeState();//TODO niewiadomo gdzie to uzyc
protected:
	Vector3* acceleration;
	Vector3* velocity;
	Aims* aims;
	double mass;
	double maxSpeed;
	double maxSeparationDistance;
	double minSpeed;
	int aimIndex = 0;
private:
	void addAim(ActionParameter* actionParameter);
	void removeAim();
	void attackIfCloseEnough(double& minDistance, Physical* entityClosest);

	UnitStateType unitState;
	UnitType unitType;

	db_unit* dbUnit;
	StateManager* states;

	double unitRadius = 2;
	double attackIntrest = 10;
	static double hbMaxSize;
};
