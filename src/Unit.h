#pragma once

#include <vector>
#include "ObjectEnums.h"
#include "defines.h"
#include "Game.h"
#include "Entity.h"
#include "UnitStateType.h"
#include "UnitType.h"
#include <Urho3D/Graphics/BillboardSet.h>
#include "Physical.h"

class State;

class Unit : public Physical
{
public:
	Unit(Vector3* _position, Urho3D::Node* _boxNode);
	~Unit();
	double getHealthBarSize();
	void populate(db_unit* _dbUnit);
	void move(double timeStep);
	void applyForce(double timeStep);
	int getSubType() override;
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
	void updateHeight(double y, double timeStep);
	void updateRotation();
protected:
	Vector3* acceleration;
	Vector3* velocity;
	double mass;
	double maxSpeed;
	double maxSeparationDistance;
	double minSpeed;
	Aims* aims;
	Vector3* aimPosition;
	int aimIndex = 0;
private:
	void appendAim(ActionParameter* actionParameter);
	void addAim(ActionParameter* actionParameter);
	void followAim(ActionParameter* parameter);

	UnitStateType unitState;
	State** states;
	UnitType unitType;

	db_unit* dbUnit;

	double unitRadius = 2;
	double hpCoef = 100;
	double maxHpCoef = 100;
	double attackCoef = 10;
	double attackRange;//TODO Range czy pojedyncze?
	double defenseCoef = 0.3;
	double attackSpeed = 1;
	double attackIntrest = 10;
	static double hbMaxSize;
};
