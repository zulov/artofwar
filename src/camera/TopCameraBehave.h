#pragma once
#include "CameraBehave.h"

class TopCameraBehave : public CameraBehave
{
public:
	TopCameraBehave();
	~TopCameraBehave() override;
	void translate(bool cameraKeys[], int wheel, float timeStep, float min) override;
	void rotate(const IntVector2& mouseMove, double mouse_sensitivity) override;

	void setRotation(const Urho3D::Quaternion& rotation) override;
	Urho3D::String* getInfo() override;
	MouseMode getMouseMode() override;
private:
	float maxY = 200;
	float orthoSize = 100;
};

