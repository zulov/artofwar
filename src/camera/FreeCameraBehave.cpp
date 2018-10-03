#include "FreeCameraBehave.h"
#include "Game.h"


FreeCameraBehave::FreeCameraBehave(): CameraBehave(Urho3D::Vector3(0.0f, 50.0f, -50.0f), 3, "FreeCam") {
}


FreeCameraBehave::~FreeCameraBehave() = default;

void FreeCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep, float min) {
	if (cameraKeys[0]) {
		translateCam(timeStep, 5, Urho3D::Vector3::FORWARD);
	}
	if (cameraKeys[1]) {
		translateCam(timeStep, 1, Urho3D::Vector3::BACK);
	}
	if (cameraKeys[2]) {
		translateCam(timeStep, 3, Urho3D::Vector3::LEFT);
	}
	if (cameraKeys[3]) {
		translateCam(timeStep, 3, Urho3D::Vector3::RIGHT);
	}
}

void FreeCameraBehave::rotate(const Urho3D::IntVector2& mouseMove, const double mouse_sensitivity) {
	yaw += mouse_sensitivity * mouseMove.x_;
	pitch += mouse_sensitivity * mouseMove.y_;
	//pitch_ = Clamp(pitch_, -90.0f, 90.0f);
	setRotation(Urho3D::Quaternion(pitch, yaw, 0.0f));
}

void FreeCameraBehave::setRotation(const Urho3D::Quaternion& rotation) {
	cameraNode->SetRotation(rotation);
}

Urho3D::String* FreeCameraBehave::getInfo() {
	if (changed) {
		*info = name + " \t" + cameraNode->GetPosition().ToString();
		changed = false;
	}
	return info;
}

Urho3D::MouseMode FreeCameraBehave::getMouseMode() {
	return Urho3D::MM_RELATIVE;
}
