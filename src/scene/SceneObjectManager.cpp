#include "SceneObjectManager.h"
#include "objects/Entity.h"
#include "utils.h"


SceneObjectManager::SceneObjectManager() = default;


SceneObjectManager::~SceneObjectManager() {
	clear_vector(entities);
}

void SceneObjectManager::add(Entity* entity) {
	entities.push_back(entity);
}
