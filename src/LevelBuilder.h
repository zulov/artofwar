#pragma once
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include "Entity.h"

class ObjectManager;
using namespace Urho3D;

class LevelBuilder {
public:
	LevelBuilder(ResourceCache* _cache);
	~LevelBuilder();
	SharedPtr<Scene> CreateScene(Context* context, ObjectManager* objectManager);

private:
	SharedPtr<Scene> scene;
	ResourceCache* cache;
	Entity* createZone();
	Entity* createLight();
	Entity* createGround();
};

