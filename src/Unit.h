#pragma once

#include <vector>
#include "ObjectEnums.h"
#include "defines.h"
#include "Game.h"
#include "Entity.h"
#include "UnitState.h"

using namespace Urho3D;

class Unit : public Entity
{
public:
	Unit(Vector3* _position, Urho3D::Node* _boxNode, Font* _font);
	~Unit();
	void populate(db_unit* definition);
	void move(double timeStep);
	void applyForce(double timeStep);
	ObjectType getType() override;
	void select() override;
	void unSelect() override;
	void setAcceleration(Vector3* _acceleration);
	double getMaxSeparationDistance();

	void action(ActionType actionType, ActionParameter* parameter) override;
	Vector3* getAim();
	Vector3* getVelocity();
	double getMass();
	double getUnitRadius();
	void absorbAttack(double attackCoef) override;
	void attack(vector<Entity*>* enemies);
	void attack(Entity *enemy);
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
	Vector3 *aimPosition;
	int aimIndex = 0;
private:
	void appendAim(ActionParameter* actionParameter);
	void addAim(ActionParameter* actionParameter);
	void followAim(ActionParameter* parameter);

	double unitRadius = 2;
	Node* healthBar;
	UnitState unitState;

	double hpCoef = 100;
	double maxHpCoef = 100;
	double attackCoef = 10;
	double attackRange;//TODO Range czy pojedyncze?
	double defenseCoef = 0.3;
	double attackSpeed = 1;
	double attackIntrest = 10;
};
