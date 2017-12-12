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

int Entity::getDbID() {
	return -1;
}

void Entity::action(ActionType actionType, ActionParameter* parameter) {
}

std::string Entity::getValues(int precision) {
	return std::to_string(getDbID()) + ","
		+ std::to_string((int)alive) + ",";//TODO to pewnie nie potrzebne
}

std::string Entity::getColumns() {
	return
		"id_db				INT		NOT NULL,"
		"alive				INT		NOT NULL,";
}
