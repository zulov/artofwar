#include "Entity.h"
#include "ObjectEnums.h"
#include "Game.h"


Entity::Entity(Urho3D::Node* _boxNode, ObjectType _type) {
	node = _boxNode;	
	alive = true;
	type = _type;
}

Entity::~Entity() {
	
}

Urho3D::Node* Entity::getNode() {
	return node;
}

bool Entity::isAlive() {
	return alive;
}

ObjectType Entity::getType() {
	return type;
}

int Entity::getSubType() {
	return -1;
}

int Entity::getSubTypeId() {
	return -1;
}

void Entity::action(ActionType actionType, ActionParameter* parameter) {
}
