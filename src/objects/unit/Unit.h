#pragma once

#include <vector>
#include "ObjectEnums.h"
#include "defines.h"
#include "Game.h"
#include "objects/Entity.h"
#include "UnitStateType.h"
#include "UnitType.h"
#include <Urho3D/Graphics/BillboardSet.h>
#include "objects/Physical.h"
#include "state/StateManager.h"

class State;

class Unit : public Physical
{
	friend class State;
public:
	Unit(Vector3* _position, Urho3D::Node* _boxNode);
	~Unit();
	double getHealthBarSize() override;
	void populate(db_unit* _dbUnit);
	void checkAim();
	void move(double timeStep);
	void applyForce(double timeStep);
	int getSubTypeId() override;
	void setAcceleration(Vector3* _acceleration);
	double getMaxSeparationDistance();

	void action(ActionType actionType, ActionParameter* parameter) override;
	Vector3* getAim();
	Vector3* getVelocity();
	double getMass();
	double getUnitRadius();
	void absorbAttack(double attackCoef) override;

	void attack(vector<Physical*>* enemies);
	void attack(Physical* enemy);
	void attack();
	void updateHeight(double y, double timeStep);
	void updateRotation();
	String* toMultiLineString() override;
	void buttonAction(short id) override;
	UnitStateType getState();
	void clean() override;
protected:
	Vector3* acceleration;
	Vector3* velocity;
	double mass;
	double maxSpeed;
	double maxSeparationDistance;
	double minSpeed;
	Aims* aims;
	Physical* followTo;
	int aimIndex = 0;
private:
	void appendAim(ActionParameter* actionParameter);
	void addAim(ActionParameter* actionParameter);
	void removeAim();
	void followAim(ActionParameter* parameter);
	void attackIfCloseEnough(double& minDistance, Physical* entityClosest);
	UnitStateType unitState;

	UnitType unitType;

	db_unit* dbUnit;
	StateManager* states;
	double unitRadius = 2;

	double attackIntrest = 10;
	static double hbMaxSize;
};
