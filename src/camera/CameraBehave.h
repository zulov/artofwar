#pragma once
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Scene/Node.h>


class CameraBehave
{
public:
	CameraBehave(const Urho3D::Vector3& pos, float minY, const Urho3D::String& name);
	virtual ~CameraBehave();

	virtual void translate(bool cameraKeys[], int wheel, float timeStep, float min) = 0;
	virtual void rotate(const Urho3D::IntVector2& mouseMove, double mouse_sensitivity) = 0;
	virtual void setRotation(const Urho3D::Quaternion& rotation) = 0;
	Urho3D::Camera* getComponent();
	virtual Urho3D::String* getInfo() = 0; //TODO da sie to uwspolnic
	virtual Urho3D::MouseMode getMouseMode() = 0;
	const Urho3D::Vector3& getPosition();
	void changePosition(float percentX, float percentY);
protected:
	Urho3D::Node* cameraNode;
	Urho3D::String* info;
	bool changed;
	Urho3D::Camera* camera;
	float minY;
	Urho3D::String name;
};
