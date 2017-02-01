#pragma once
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Vector3.h>

using namespace Urho3D;
class Unit
{
public:
	Unit(Vector3 _position, Urho3D::Node* _boxNode);
	~Unit();

	void move(double timeStep);
	void applyForce(double timeStep);
	void setAcceleration(Vector3 _acceleration);
	Vector3 getPosition();
	double getMaxSeparationDistance();
	double getMinimalDistance();
	Urho3D::Node * getNode();
protected:
	Urho3D::Node* node;
	Vector3 position;
	Vector3 acceleration;
	Vector3 velocity;
	Vector3 rotation;

	double mass;
	double maxSpeed;
	double maxSeparationDistance;
	double minimalDistance;


};

