#pragma once

namespace Urho3D {
	class Node;
}


class SceneObjectManager {
public:
	SceneObjectManager() = default;
	~SceneObjectManager();

	void setGround(Urho3D::Node* ground);
	void setLight(Urho3D::Node* light);
	void setZone(Urho3D::Node* zone);
private:
	Urho3D::Node* ground{};
	Urho3D::Node* light{};
	Urho3D::Node* zone{};
};
