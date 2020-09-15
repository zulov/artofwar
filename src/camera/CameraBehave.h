#pragma once
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Input/Input.h>


class CameraBehave {
public:
	CameraBehave(const Urho3D::Vector3& pos, float minY, Urho3D::String name);
	virtual ~CameraBehave();

	virtual void translate(bool cameraKeys[], int wheel, float timeStep, float min) = 0;
	virtual void rotate(const Urho3D::IntVector2& mouseMove, float mouse_sensitivity) = 0;
	virtual void setRotation(const Urho3D::Quaternion& rotation) = 0;
	Urho3D::Camera* getComponent() const;
	virtual Urho3D::String* getInfo() = 0; //TODO da sie to uwspolnic
	virtual Urho3D::MouseMode getMouseMode() = 0;
	const Urho3D::Vector3& getPosition() const;
	void changePosition(float percentX, float percentY);
protected:
	void translateCam(float timeStep, float diff, Urho3D::Vector3 dir);
	void translateInternal(const bool* cameraKeys, float timeStep, float diff);
	Urho3D::Node* cameraNode;
	Urho3D::String* info;
	Urho3D::Camera* camera;

	Urho3D::String name;
	Urho3D::Vector3 dirs[4] = {
		Urho3D::Vector3::FORWARD, Urho3D::Vector3::BACK, Urho3D::Vector3::LEFT, Urho3D::Vector3::RIGHT
	};
	bool changed;
	float minY;
	float coefs[4];
};
