#include "Entity.h"
#include "ObjectEnums.h"
#include "Game.h"


Entity::Entity(Urho3D::Node* _node, ObjectType _type) {
	node = _node;	
	alive = true;
	type = _type;
}

Entity::~Entity() {
	node->Remove();
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

int Entity::getSubTypeId() {
	return -1;
}

void Entity::action(ActionType actionType, ActionParameter* parameter) {
}
