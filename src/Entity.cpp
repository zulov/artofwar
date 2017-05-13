#include "Entity.h"
#include "ObjectEnums.h"


Entity::Entity(Vector3* _position, Urho3D::Node* _boxNode, Font* _font) {
	node = _boxNode;
	position = _position;
	rotation = new Vector3();
	alive = true;
	font = _font;
	bucketZ = bucketX = INT_MIN;
}

Entity::~Entity() {
	delete position;
	delete rotation;
}

double Entity::getMinimalDistance() {
	return minimalDistance;
}

Urho3D::Node* Entity::getNode() {
	return node;
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

int Entity::getType() {
	return ENTITY;
}

void Entity::select() {
}

void Entity::unSelect() {
}

void Entity::action(ActionType actionType, ActionParameter* parameter) {
}

Aims* Entity::getAims() {
	return nullptr;
}

Urho3D::Vector3* Entity::getPosition() {
	return position;
}
