#include "RtsCameraBehave.h"
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Input/Input.h>

RtsCameraBehave::RtsCameraBehave(): CameraBehave(Urho3D::Vector3(0.0f, 50.0f, -50.0f), 3, "RTSCam") {
	const auto diff = sqrt(50.0f - minY) + 1.f;
	cameraNode->SetDirection(Urho3D::Vector3::DOWN * diff + Urho3D::Vector3::FORWARD * 10.f);
}

bool RtsCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep, float min) {
	Urho3D::Vector3 pos = cameraNode->GetWorldPosition();
	const float localMin = minY + min;
	const float localMax = maxY + min;

	float diff;
	if (pos.y_ < localMin) {
		diff = 1;
	} else {
		diff = sqrt((pos.y_ - localMin) / 10) + 1.f;
	}
	bool hasChange = translateInternal(cameraKeys, timeStep, diff);

	if (wheel != 0 || pos.y_ < localMin) {
		Urho3D::Vector3 pos2 = cameraNode->GetWorldPosition();
		if (pos2.y_ < localMin) {
			pos2.y_ = localMin;
		}
		float diff = sqrt(pos2.y_ - localMin) + 1.f;

		pos2 += Urho3D::Vector3::DOWN * wheel * diff * 1.5f;

		if (pos2.y_ < localMin) {
			pos2.y_ = localMin;
		} else if (pos2.y_ > localMax) {
			pos2.y_ = localMax;
		}

		cameraNode->SetWorldPosition(pos2);
		cameraNode->SetDirection(Urho3D::Vector3::DOWN * diff + Urho3D::Vector3::FORWARD * 10);
		hasChange = true;
	}
	return hasChange;
}


Urho3D::MouseMode RtsCameraBehave::getMouseMode() {
	return Urho3D::MM_RELATIVE;
}

Urho3D::Vector2 RtsCameraBehave::getTargetPos() const {
	auto pos = cameraNode->GetPosition();

	return Urho3D::Vector2(pos.x_, pos.z_ + 100.f); //TODO hardcoded
}
