#pragma once
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Input/Input.h>

using namespace Urho3D;

class CameraBehave {
public:
	CameraBehave();
	~CameraBehave();

	virtual void translate(bool cameraKeys[], int wheel, float timeStep) = 0;
	virtual void rotate() = 0;
	virtual void setRotate(const Urho3D::Quaternion& rotation) = 0;
	Urho3D::Camera * getComponent();
	virtual Urho3D::String getInfo() = 0;
	virtual MouseMode getMouseMode() = 0;
protected:
	Urho3D::SharedPtr<Urho3D::Node> cameraNode;
	Urho3D::String name;
public:
	
};

