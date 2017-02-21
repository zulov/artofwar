#pragma once
#include "CameraBehave.h"
using namespace Urho3D;

class FreeCameraBehave : public CameraBehave
{
public:
	FreeCameraBehave(Urho3D::Context* context);
	~FreeCameraBehave();

	virtual void translate(bool cameraKeys[], int wheel, float timeStep);
	virtual void rotate(const IntVector2& mouseMove, const double mouse_sensitivity);
	virtual void setRotation(const Urho3D::Quaternion& rotation);
	virtual Urho3D::String getInfo();
	virtual MouseMode getMouseMode();
private:
	double yaw = 0.0;
	double pitch = 0.0;
};
