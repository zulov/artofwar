#include "Entity.h"
#include "ObjectEnums.h"


Entity::Entity(Vector3* _position, Urho3D::Node* _boxNode) {
	node = _boxNode;
	position = _position;
	rotation = new Vector3();
	alive = true;
	for (int i = 0; i < BUCKET_SET_NUMBER; ++i) {
		bucketZ[i] = bucketX[i] = INT_MIN;
	}
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

signed char Entity::getBucketX(signed char param) {
	return bucketX[param];
}

signed char Entity::getBucketZ(signed char param) {
	return bucketZ[param];
}

bool Entity::isAlive() {
	return alive;
}

bool Entity::bucketHasChanged(short int posX, short int posZ, short int param) {
	if (bucketX[param] == posX && bucketZ[param] == posZ) {
		return false;
	} else {
		return true;
	}
}

void Entity::setBucket(short int posX, short int posZ, short int param) {
	bucketX[param] = posX;
	bucketZ[param] = posZ;
}

void Entity::setTeam(signed char _team) {
	team = _team;
}

void Entity::setPlayer(signed char player) {
	this->player = player;
}

ObjectType Entity::getType() {
	return ENTITY;
}

int Entity::getSubType() {
	return -1;
}

void Entity::select() {
}

void Entity::unSelect() {
}

void Entity::action(ActionType actionType, ActionParameter* parameter) {
}

signed char Entity::getTeam() {
	return team;
}

void Entity::absorbAttack(double attackCoef) {
	
}

Urho3D::Vector3* Entity::getPosition() {
	return position;
}
