#pragma once
#include <Urho3D/Graphics/Camera.h>

namespace Urho3D {
	class Quaternion;
	class IntVector2;
	class String;
	class Vector3;
	enum MouseMode;
}


class CameraBehave {
public:
	CameraBehave(const Urho3D::Vector3& pos, float minY, Urho3D::String name);
	virtual ~CameraBehave();

	virtual bool translate(bool cameraKeys[], int wheel, float timeStep, float min) = 0;
	virtual bool rotate(const Urho3D::IntVector2& mouseMove, float mouse_sensitivity) { return false; }

	virtual void setRotation(const Urho3D::Quaternion& rotation) {}
	Urho3D::Camera* getComponent() const;
	Urho3D::String getInfo() const;
	virtual Urho3D::MouseMode getMouseMode() = 0;
	const Urho3D::Vector3& getPosition() const;
	void changePositionInPercent(float percentX, float percentY) const;
	void changePosition(float x, float z) const;
	void setPos2D(const Urho3D::Vector3& newPos) const;
	virtual Urho3D::Vector2 getTargetPos() const;
protected:
	void translateCam(float timeStep, float diff, Urho3D::Vector3 dir) const;
	bool translateInternal(const bool* cameraKeys, float timeStep, float diff);
	Urho3D::Node* cameraNode;
	Urho3D::Camera* camera;

	Urho3D::String name;

	float minY;
	float coefs[4];
};
