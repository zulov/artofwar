#include "SceneObjectManager.h"
#include <Urho3D/Scene/Node.h>


SceneObjectManager::~SceneObjectManager() {
	ground->Remove();
	light->Remove();
	zone->Remove();
}

void SceneObjectManager::setGround(Urho3D::Node* ground) {
	this->ground = ground;
}

void SceneObjectManager::setLight(Urho3D::Node* light) {
	this->light = light;
}

void SceneObjectManager::setZone(Urho3D::Node* zone) {
	this->zone = zone;
}
