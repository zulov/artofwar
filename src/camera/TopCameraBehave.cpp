#include "TopCameraBehave.h"

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Input/Input.h>

constexpr float TOP_CAM_MAX_Y = 200.f;

TopCameraBehave::TopCameraBehave(): CameraBehave(Urho3D::Vector3(0, 50, 0), 20, "TopCam") {
	const auto diff = sqrt(50.0f - minY) + 1;
	cameraNode->SetDirection(Urho3D::Vector3::DOWN * diff);
	camera->SetOrthographic(true);
}

bool TopCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep, float min) {
	bool hasMoved = translateInternal(cameraKeys, timeStep, sqrt((orthoSize - minY) / 10.f) + 1.f);
	if (wheel != 0) {
		orthoSize -= wheel * (sqrt(orthoSize - minY) + 1) * 1.5f;

		if (orthoSize < minY) {
			orthoSize = minY;
		} else if (orthoSize > TOP_CAM_MAX_Y) {
			orthoSize = TOP_CAM_MAX_Y;
		}
		camera->SetOrthoSize(orthoSize);

		hasMoved = true;
	}
	return hasMoved;
}

Urho3D::MouseMode TopCameraBehave::getMouseMode() {
	return Urho3D::MM_RELATIVE;
}
