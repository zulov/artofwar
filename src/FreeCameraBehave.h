#pragma once
#include "CameraBehave.h"
using namespace Urho3D;

class FreeCameraBehave : public CameraBehave
{
public:
	FreeCameraBehave();
	~FreeCameraBehave();

	void translate(bool cameraKeys[], int wheel, float timeStep) override;
	void rotate(const IntVector2& mouseMove, const double mouse_sensitivity) override;
	void setRotation(const Urho3D::Quaternion& rotation) override;
	Urho3D::String* getInfo() override;
	MouseMode getMouseMode() override;
private:
	double yaw = 0.0;
	double pitch = 0.0;
};
