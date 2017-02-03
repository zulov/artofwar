#pragma once
#include "Entity.h"

using namespace Urho3D;

class Unit : public Entity
{
public:
	Unit(Vector3 _position, Urho3D::Node* _boxNode);
	~Unit();

	void move(double timeStep);
	void applyForce(double timeStep);
	void setAcceleration(Vector3 _acceleration);
	double getMaxSeparationDistance();

protected:
	Vector3 acceleration;
	Vector3 velocity;
	double mass;
	double maxSpeed;
	double maxSeparationDistance;

};

