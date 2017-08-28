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
	double getHealthBarSize() override;
	void populate(db_unit* _dbUnit);
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
	void updateHeight(double y, double timeStep);
	void updateRotation();
	String toMultiLineString() override;
	void buttonAction(short id) override;
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

	double attackIntrest = 10;
	static double hbMaxSize;
};
