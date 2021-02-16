#pragma once

#include <Urho3D/Container/Ptr.h>

namespace Urho3D {
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
	Urho3D::Node* createGround(const Urho3D::String& xmlName);

	Urho3D::Terrain* terrain;
	Urho3D::Node* ground{};
	Urho3D::Node* light{};
	Urho3D::Node* zone{};
};
