#include "EntityFactory.h"



EntityFactory::EntityFactory(ResourceCache* _cache, SharedPtr<Urho3D::Scene> _scene) {
	cache = _cache;
	scene = _scene;
}

EntityFactory::~EntityFactory() {}
