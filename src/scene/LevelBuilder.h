#pragma once

#include <Urho3D/Container/Ptr.h>

namespace Urho3D {
	class Scene;
	class Terrain;
}

class SceneLoader;
struct NewGameForm;

class LevelBuilder {
public:
	LevelBuilder();
	~LevelBuilder();
	void createScene(SceneLoader& loader);
	void createMap(int mapId, float spacing);
	void createScene(NewGameForm* form);
	Urho3D::Terrain* getTerrain() const;
private:
	Urho3D::SharedPtr<Urho3D::Scene> scene;
	void createGround(const Urho3D::String& xmlName, float spacing);

	Urho3D::Terrain* terrain;
};
