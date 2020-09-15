#include "TopCameraBehave.h"

#include <Urho3D/Scene/Node.h>

TopCameraBehave::TopCameraBehave(): CameraBehave(Urho3D::Vector3(0, 50, 0), 20, "TopCam") {
	const auto diff = sqrt(50.0f - minY) + 1;
	cameraNode->SetDirection(Urho3D::Vector3::DOWN * diff);
	camera->SetOrthographic(true);
}

void TopCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep, float min) {
	translateInternal(cameraKeys, timeStep, sqrt((orthoSize - minY) / 10.f) + 1.f);
	if (wheel != 0) {
		orthoSize -= wheel * (sqrt(orthoSize - minY) + 1) * 1.5f;

		if (orthoSize < minY) {
			orthoSize = minY;
		} else if (orthoSize > maxY) {
			orthoSize = maxY;
		}
		camera->SetOrthoSize(orthoSize);

		changed = true;
	}
}

void TopCameraBehave::rotate(const Urho3D::IntVector2& mouseMove, const float mouse_sensitivity) {
}

void TopCameraBehave::setRotation(const Urho3D::Quaternion& rotation) {
}

Urho3D::String* TopCameraBehave::getInfo() {
	if (changed) {
		*info = name + " \t" + cameraNode->GetPosition().ToString() + "\n" + cameraNode->GetRotation().ToString();
		changed = false;
	}
	return info;
}

Urho3D::MouseMode TopCameraBehave::getMouseMode() {
	return Urho3D::MM_RELATIVE;
}
