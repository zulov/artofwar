#pragma once
#include "CameraBehave.h"

class RtsCameraBehave : public CameraBehave {
public:
	RtsCameraBehave();
	virtual ~RtsCameraBehave() = default;
	bool translate(bool cameraKeys[], int wheel, float timeStep, float min) override;

	bool rotate(const Urho3D::IntVector2& mouseMove, float mouse_sensitivity) override { return false; }

	void setRotation(const Urho3D::Quaternion& rotation) override {
	}

	Urho3D::MouseMode getMouseMode() override;
	Urho3D::Vector2 getTargetPos() const override;
private:
	float maxY = 200;

};
