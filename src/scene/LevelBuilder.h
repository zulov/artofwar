#pragma once
#include "SceneObjectManager.h"
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Scene/Scene.h>


class SceneLoader;
struct NewGameForm;

class LevelBuilder
{
public:
	LevelBuilder();
	~LevelBuilder();
	void createScene(SceneLoader& loader);
	void createMap(int mapId);
	void createScene(NewGameForm* form);
	Urho3D::Terrain* getTerrian() const;
private:
	Urho3D::SharedPtr<Urho3D::Scene> scene;

	Entity* createZone();
	Entity* createLight(const Urho3D::Vector3& direction, const Urho3D::Color& color, Urho3D::LightType lightType);
	Entity* createGround(const Urho3D::String& heightMap, const Urho3D::String& texture, float horScale, float verScale);

	SceneObjectManager *objectManager;
	Urho3D::Terrain* terrain;
};
