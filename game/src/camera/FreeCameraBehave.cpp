#include "FreeCameraBehave.h"
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Input/Input.h>

FreeCameraBehave::FreeCameraBehave(): CameraBehave(Urho3D::Vector3(0.f, 50.f, -50.f), 3, "FreeCam") {
	coefs[0] = 4.f;
	coefs[1] = 1.f;
	coefs[2] = 3.f;
	coefs[3] = 3.f;
}

bool FreeCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep, float min) {
	return translateInternal(cameraKeys, timeStep, 1);
}

bool FreeCameraBehave::rotate(const Urho3D::IntVector2& mouseMove, const float mouse_sensitivity) {
	if (mouseMove != Urho3D::IntVector2::ZERO) {
		yaw += mouse_sensitivity * mouseMove.x_;
		pitch += mouse_sensitivity * mouseMove.y_;
		//pitch_ = Clamp(pitch_, -90.0f, 90.0f);
		cameraNode->SetRotation(Urho3D::Quaternion(pitch, yaw, 0.0f));
		return true;
	}
	return false;
}

Urho3D::MouseMode FreeCameraBehave::getMouseMode() {
	return Urho3D::MM_RELATIVE;
}
