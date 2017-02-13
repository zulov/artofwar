#pragma once
#include "CameraBehave.h"
using namespace Urho3D;

class FreeCameraBehave : public CameraBehave {
public:
	FreeCameraBehave(Urho3D::Context* context);
	~FreeCameraBehave();

	virtual void translate(bool cameraKeys[], int wheel, float timeStep);
	virtual void rotate();
	virtual void setRotate(const Urho3D::Quaternion& rotation);
	virtual Urho3D::String getInfo();
	virtual MouseMode getMouseMode();
};

