#pragma once
#include "SceneObjectManager.h"
#include "load/SceneLoader.h"
#include "objects/Entity.h"
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Scene/Scene.h>
#include "hud/window/main_menu/new_game/NewGameForm.h"


using namespace Urho3D;

class LevelBuilder
{
public:
	LevelBuilder();
	~LevelBuilder();
	void createScene(SceneLoader& loader);
	void createMap(int mapId);
	void createScene(NewGameForm* form);
	Terrain* getTerrian();
private:
	SharedPtr<Urho3D::Scene> scene;

	Entity* createZone();
	Entity* createLight(const Vector3& direction, const Color& color, LightType lightType);
	Entity* createGround(const String& heightMap, const String& texture, float horScale, float verScale);

	SceneObjectManager *objectManager;
	Terrain* terrain;
};
