#pragma once
#include "camera/CameraBehave.h"

class FreeCameraBehave : public CameraBehave
{
public:
	FreeCameraBehave();
	virtual ~FreeCameraBehave() = default;

	void translate(bool cameraKeys[], int wheel, float timeStep, float min) override;
	void rotate(const Urho3D::IntVector2& mouseMove, float mouse_sensitivity) override;
	void setRotation(const Urho3D::Quaternion& rotation) override;
	Urho3D::String* getInfo() override;
	Urho3D::MouseMode getMouseMode() override;
private:
	float yaw = 0.0;
	float pitch = 0.0;
};
