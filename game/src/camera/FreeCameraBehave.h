#pragma once
#include "camera/CameraBehave.h"

class FreeCameraBehave : public CameraBehave {
public:
	FreeCameraBehave();
	virtual ~FreeCameraBehave() = default;

	bool translate(bool cameraKeys[], int wheel, float timeStep, float min) override;
	bool rotate(const Urho3D::IntVector2& mouseMove, float mouse_sensitivity) override;

	Urho3D::MouseMode getMouseMode() override;
private:
	float yaw = 0.f;
	float pitch = 0.f;
};
