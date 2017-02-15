#include "Entity.h"



Entity::Entity(Vector3 _position, Urho3D::Node* _boxNode) {
	node = _boxNode;
	position = _position;
	rotation = Vector3();
	minimalDistance = 0.71;
	alive = true;
	bucketLevel = 2;

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

int Entity::getBucketZ() {
	return bucketZ;
}

bool Entity::isAlive() {
	return alive;
}

bool Entity::bucketHasChanged(int posX, int posZ) {
	if (bucketX == posX && bucketZ == posZ) {
		return false;
	} else {
		return true;
	}
}

void Entity::setBucket(int posX, int posZ) {
	bucketX = posX;
	bucketZ = posZ;
}

Urho3D::Vector3 Entity::getPosition() {
	return position;
}
