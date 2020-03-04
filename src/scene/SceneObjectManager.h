#pragma once
#include "objects/Physical.h"

class Entity;

class SceneObjectManager {
public:
	SceneObjectManager();
	~SceneObjectManager();

	void setGround(Physical* ground);
	void setLight(Urho3D::Node* light);
	void setZone(Urho3D::Node* zone);
private:
	Physical* ground;
	Urho3D::Node* light;
	Urho3D::Node* zone;


};
