#include "SceneObjectManager.h"
#include "utils.h"
#include "objects/Entity.h"


SceneObjectManager::SceneObjectManager() {
}


SceneObjectManager::~SceneObjectManager() {
	clear_vector(entities);
}

void SceneObjectManager::add(Entity* entity) {
	entities.push_back(entity);
}
