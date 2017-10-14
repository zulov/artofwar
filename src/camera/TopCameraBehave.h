#pragma once
#include "CameraBehave.h"

class TopCameraBehave : public CameraBehave
{
public:
	TopCameraBehave();
	~TopCameraBehave();
	void translate(bool cameraKeys[], int wheel, float timeStep) override;
	void rotate(const IntVector2& mouseMove, const double mouse_sensitivity) override;

	void setRotation(const Urho3D::Quaternion& rotation) override;
	Urho3D::String* getInfo() override;
	MouseMode getMouseMode() override;
private:
	float maxY = 200, minY = 20;
	float orthoSize = 100;
};

