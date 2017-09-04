#include "SceneObjectManager.h"


SceneObjectManager::SceneObjectManager() {
	entities = new std::vector<Entity*>();
}


SceneObjectManager::~SceneObjectManager() {
	clear_vector(entities);
	delete entities;
}

void SceneObjectManager::add(Entity* entity) {
	entities->push_back(entity);
}
