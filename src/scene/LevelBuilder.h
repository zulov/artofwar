#pragma once
#include <Urho3D/Graphics/Light.h>
#include "SceneObjectManager.h"

namespace Urho3D {
	class Color;
	class Node;
	class Scene;
	class Terrain;
	class Vector3;
}

class Physical;
class SceneLoader;
struct NewGameForm;

class LevelBuilder {
public:
	LevelBuilder();
	~LevelBuilder();
	void createScene(SceneLoader& loader);
	void createMap(int mapId);
	void createScene(NewGameForm* form);
	Urho3D::Terrain* getTerrain() const;
private:
	Urho3D::SharedPtr<Urho3D::Scene> scene;

	Urho3D::Node* createZone();
	Urho3D::Node* createLight(const Urho3D::Vector3& direction, const Urho3D::Color& color,
	                          Urho3D::LightType lightType);
	void setMaterial(const Urho3D::String& texture) const;
	Physical* createGround(const Urho3D::String& heightMap, const Urho3D::String& texture, float horScale,
	                       float verScale);

	SceneObjectManager* objectManager;
	Urho3D::Terrain* terrain;
};
