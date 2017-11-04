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
#include "objects/Entity.h"
#include "Game.h"
#include "SceneObjectManager.h"
#include "MapReader.h"


using namespace Urho3D;

class LevelBuilder
{
public:
	LevelBuilder(SceneObjectManager* _objectManager);
	~LevelBuilder();
	void createScene();
	void execute();
	

private:
	SharedPtr<Urho3D::Scene> scene;

	Entity* createZone();
	Entity* createLight();
	Entity* createGround();
	//BuildList* buildList;
	SceneObjectManager* objectManager;
	MapReader * mapReader;

};
