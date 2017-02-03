#include "Entity.h"



Entity::Entity(Vector3 _position, Urho3D::Node* _boxNode) {
	node = _boxNode;
	position = _position;
	rotation = Vector3();
	minimalDistance = 0.71;

}


Entity::~Entity() {}

double Entity::getMinimalDistance() {
	return minimalDistance;
}

Urho3D::Node * Entity::getNode() {
	return node;
}

Urho3D::Vector3 Entity::getPosition() {
	return position;
}
