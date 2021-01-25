#include "SceneObjectManager.h"
#include <Urho3D/Scene/Node.h>


SceneObjectManager::~SceneObjectManager() {
	ground->Remove();
	if (light) { light->Remove(); }
	if (zone) { zone->Remove(); }
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
