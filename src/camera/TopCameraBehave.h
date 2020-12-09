#pragma once
#include "CameraBehave.h"


class TopCameraBehave : public CameraBehave {
public:
	TopCameraBehave();
	~TopCameraBehave() override = default;
	bool translate(bool cameraKeys[], int wheel, float timeStep, float min) override;

	Urho3D::MouseMode getMouseMode() override;
private:	
	float orthoSize = 100;
};
