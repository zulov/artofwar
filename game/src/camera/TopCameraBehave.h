#pragma once
#include "CameraBehave.h"


class TopCameraBehave : public CameraBehave {
public:
	TopCameraBehave();
	~TopCameraBehave() override = default;
	bool translate(bool cameraKeys[], int wheel, float timeStep, float min) override;

	CameraBehaviorType getType() const override { return CameraBehaviorType::TOP; }
	Urho3D::MouseMode getMouseMode() override;
private:	
	float orthoSize = 100;
};
