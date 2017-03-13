#pragma once
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include "LinkComponent.h"


using namespace Urho3D;

class EntityFactory {
public:
	EntityFactory(ResourceCache* cache, SharedPtr<Urho3D::Scene> scene);
	~EntityFactory();
	
protected:
	ResourceCache* cache;
	SharedPtr<Urho3D::Scene> scene;

};

