#include "SceneObjectManager.h"
#include "utils.h"
#include "objects/Entity.h"


SceneObjectManager::SceneObjectManager() = default;


SceneObjectManager::~SceneObjectManager() {
	clear_vector(entities);
}

void SceneObjectManager::add(Entity* entity) {
	entities.push_back(entity);
}
