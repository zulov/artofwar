#include "Unit.h"

Unit::Unit(Vector3 _position, Urho3D::Node* _boxNode){
	node = _boxNode;
	position = _position;
	acceleration = Vector3();
	velocity = Vector3();
	rotation = Vector3();
	mass = 1;
	maxSpeed = 1;
	minimalDistance = 0.71;
	maxSeparationDistance = 4;
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

Urho3D::Vector3 Unit::getPosition() {
	return position;
}

double Unit::getMaxSeparationDistance() {
	return maxSeparationDistance;
}

double Unit::getMinimalDistance() {
	return minimalDistance;
}

Urho3D::Node * Unit::getNode() {
	return node;
}

void Unit::applyForce(double timeStep) {
	double coef = timeStep / mass;
	velocity += acceleration*coef;
	if (velocity.Length() > maxSpeed*maxSpeed) {
		velocity.Normalize();
		velocity *= maxSpeed;
	}
}