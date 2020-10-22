#pragma once

namespace Urho3D {
	class Node;
}

class Physical;

class SceneObjectManager {
public:
	SceneObjectManager() = default;
	~SceneObjectManager();

	void setGround(Physical* ground);
	void setLight(Urho3D::Node* light);
	void setZone(Urho3D::Node* zone);
private:
	Physical* ground;
	Urho3D::Node* light;
	Urho3D::Node* zone;
};
