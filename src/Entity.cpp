#include "Entity.h"
#include "ObjectEnums.h"


Entity::Entity(Vector3* _position, Urho3D::Node* _boxNode, Font* _font) {
	node = _boxNode;
	position = _position;
	rotation = new Vector3();
	alive = true;
	font = _font;
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

int Entity::getBucketX(int param) {
	return bucketX[param];
}

int Entity::getBucketZ(int param) {
	return bucketZ[param];
}

bool Entity::isAlive() {
	return alive;
}

bool Entity::bucketHasChanged(int posX, int posZ, int param) {
	if (bucketX[param] == posX && bucketZ[param] == posZ) {
		return false;
	} else {
		return true;
	}
}

void Entity::setBucket(int posX, int posZ, int param) {
	bucketX[param] = posX;
	bucketZ[param] = posZ;
}

void Entity::setTeam(int _team) {
	team = _team;
}

void Entity::setPlayer(int player) {
	this->player = player;
}

ObjectType Entity::getType() {
	return ENTITY;
}

void Entity::select() {
}

void Entity::unSelect() {
}

void Entity::action(ActionType actionType, ActionParameter* parameter) {
}

int Entity::getTeam() {
	return team;
}

void Entity::absorbAttack(double attackCoef) {
	
}

Urho3D::Vector3* Entity::getPosition() {
	return position;
}
