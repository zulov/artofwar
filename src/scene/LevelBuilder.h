#pragma once
#include "SceneObjectManager.h"
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Scene/Scene.h>


class SceneLoader;
struct NewGameForm;
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
