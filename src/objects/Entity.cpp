#include "Entity.h"
#include "Game.h"
#include "ObjectEnums.h"
#include <Urho3D/Scene/Scene.h>
#include <string>


Entity::Entity() {
	node = Game::getScene()->CreateChild();
}

Entity::~Entity() {
	node->Remove();
}

ObjectType Entity::getType() const {
	return ObjectType::ENTITY;
}

bool Entity::isAlive() const {
	return true;
}

int Entity::getDbID() {
	return -1;
}

std::string Entity::getValues(int precision) {
	return std::to_string(getDbID()) + ","; //TODO to pewnie nie potrzebne
}

std::string Entity::getColumns() {
	return "id_db		INT		NOT NULL,";
}
