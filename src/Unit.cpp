#include "Unit.h"

Unit::Unit(Vector3 _position, Urho3D::Node* _boxNode) : Entity(_position, _boxNode) {

	maxSeparationDistance = 4;
	mass = 1;
	maxSpeed = 1;
	acceleration = Vector3();
	velocity = Vector3();
}

Unit::~Unit() {
	//delete node;
}

void Unit::move(double timeStep) {
	position += velocity*timeStep;
	node->SetPosition(position);
}

void Unit::setAcceleration(Vector3 _acceleration) {
	acceleration = _acceleration;
}

double Unit::getMaxSeparationDistance() {
	return maxSeparationDistance;
}

void Unit::applyForce(double timeStep) {
	double coef = timeStep / mass;
	velocity += acceleration*coef;
	if (velocity.Length() > maxSpeed*maxSpeed) {
		velocity.Normalize();
		velocity *= maxSpeed;
	}
}