#include "Entity.h"
#include "ObjectEnums.h"
#include "Game.h"
#include <Urho3D/Scene/Scene.h>
#include <string>


Entity::Entity(ObjectType _type) {
	node = Game::get()->getScene()->CreateChild();
	alive = true;
	type = _type;
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

int Entity::getID() {
	return -1;
}

void Entity::action(ActionType actionType, ActionParameter* parameter) {
}

std::string Entity::getValues(int precision) {
	return std::to_string(getID()) + ",";
}

std::string Entity::getColumns() {
	return "id				INT		NOT NULL,";
}
