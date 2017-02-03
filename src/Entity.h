#pragma once
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Vector3.h>

using namespace Urho3D;

class Entity {
public:
	Entity(Vector3 _position, Urho3D::Node* _boxNode);
	~Entity();
	Vector3 getPosition();
	double getMinimalDistance();
	Urho3D::Node * getNode();
protected:
	Urho3D::Node* node;
	Vector3 position;
	Vector3 rotation;
	double minimalDistance;



};

