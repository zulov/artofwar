#pragma once

#include <vector>
#include "ObjectEnums.h"
#include "defines.h"
#include "Game.h"
#include "Entity.h"

using namespace Urho3D;

class Unit : public Entity
{
public:
	Unit(Vector3* _position, Urho3D::Node* _boxNode, Font* _font);
	~Unit();

	void move(double timeStep);
	void applyForce(double timeStep);
	virtual int getType() override;
	void select() override;
	void unSelect() override;
	void setAcceleration(Vector3* _acceleration);
	double getMaxSeparationDistance();
	void action(ActionType actionType, ActionParameter* parameter) override;
	Vector3* getAim();
	Vector3* getVelocity();
	double getMass();
	double getUnitRadius();
protected:
	Vector3* acceleration;
	Vector3* velocity;
	double mass;
	double maxSpeed;
	double maxSeparationDistance;
	double minSpeed;
	Aims* aims;
	int aimIndex = 0;
	Aims* getAims() override;
private:
	void appendAim(ActionParameter* actionParameter);
	void addAim(ActionParameter* actionParameter);
	double unitRadius = 2;
};
