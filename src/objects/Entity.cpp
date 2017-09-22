#include "Entity.h"
#include "ObjectEnums.h"
#include "Game.h"
#include "LinkComponent.h"


Entity::Entity(Urho3D::Node* _node, ObjectType _type) {
	node = _node;
	alive = true;
	type = _type;
	LinkComponent* lc = node->CreateComponent<LinkComponent>();
	lc->bound(node, this);
}

Entity::~Entity() {
	node->RemoveAllChildren();
	node->RemoveAllComponents();
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
