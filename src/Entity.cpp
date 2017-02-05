#include "Entity.h"



Entity::Entity(Vector3 _position, Urho3D::Node* _boxNode) {
	node = _boxNode;
	position = _position;
	rotation = Vector3();
	minimalDistance = 0.71;
	alive = true;
	bucketLevel = 3;
}

Entity::~Entity() {}

double Entity::getMinimalDistance() {
	return minimalDistance;
}

Urho3D::Node * Entity::getNode() {
	return node;
}

int Entity::getLevelOfBucket() {
	return bucketLevel;
}

int Entity::getBucketX() {
	return bucketX;
}

int Entity::getBucketY() {
	return bucketY;
}

bool Entity::isAlive() {
	return alive;
}

bool Entity::checkBucketXY(int posX, int posY) {
	return true;//TODO
}

void Entity::setBucket(int posX, int posY) {
	bucketX = posX;
	bucketY = posY;
}

Urho3D::Vector3 Entity::getPosition() {
	return position;
}
