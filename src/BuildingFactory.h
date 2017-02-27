#pragma once
#include "EntityFactory.h"

class BuildingFactory :public EntityFactory {
public:
	BuildingFactory(ResourceCache* _cache, SharedPtr<Urho3D::Scene> _scene);
	~BuildingFactory();
};

