#pragma once
#include "CameraBehave.h"
using namespace Urho3D;

class FreeCameraBehave : public CameraBehave
{
public:
	FreeCameraBehave(Urho3D::Context* context);
	~FreeCameraBehave();

	void translate(bool cameraKeys[], int wheel, float timeStep) override;
	virtual void rotate(const IntVector2& mouseMove, const double mouse_sensitivity) override;
	virtual void setRotation(const Urho3D::Quaternion& rotation) override;
	virtual Urho3D::String getInfo();
	virtual MouseMode getMouseMode() override;
private:
	double yaw = 0.0;
	double pitch = 0.0;
};
