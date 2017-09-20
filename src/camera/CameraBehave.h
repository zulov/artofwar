#pragma once
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Input/Input.h>

using namespace Urho3D;

class CameraBehave {
public:
	CameraBehave();
	virtual ~CameraBehave();

	virtual void translate(bool cameraKeys[], int wheel, float timeStep) = 0;
	virtual void rotate(const IntVector2& mouseMove, const double mouse_sensitivity) = 0;
	virtual void setRotation(const Urho3D::Quaternion& rotation) = 0;
	Urho3D::Camera * getComponent();
	virtual Urho3D::String *getInfo() = 0;//TODO da sie to uwspolnic
	virtual MouseMode getMouseMode() = 0;
protected:
	Urho3D::SharedPtr<Urho3D::Node> cameraNode;
	Urho3D::String name;
	Urho3D::String *info;
	bool changed;
};
