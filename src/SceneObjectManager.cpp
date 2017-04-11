#include "SceneObjectManager.h"



SceneObjectManager::SceneObjectManager(){
	entities = new std::vector<Entity*>();
}


SceneObjectManager::~SceneObjectManager()
{
}

void SceneObjectManager::add(Entity* entity) {
	AbstractObjectManager::add(entity);
}
