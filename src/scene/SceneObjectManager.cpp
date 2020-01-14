#include "SceneObjectManager.h"
#include <Urho3D/Scene/Node.h>


SceneObjectManager::SceneObjectManager() = default;


SceneObjectManager::~SceneObjectManager() {
	delete ground;
	light->Remove();
	zone->Remove();
}

void SceneObjectManager::setGround(Physical* ground) {
	this->ground = ground;
}

void SceneObjectManager::setLight(Urho3D::Node* light) {
	this->light = light;
}

void SceneObjectManager::setZone(Urho3D::Node* zone) {
	this->zone = zone;
}
