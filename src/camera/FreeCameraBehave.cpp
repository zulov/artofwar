#include "FreeCameraBehave.h"
#include "Game.h"


FreeCameraBehave::FreeCameraBehave(): CameraBehave(3, "FreeCam") {
}


FreeCameraBehave::~FreeCameraBehave() = default;

void FreeCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep, float min) {
	if (cameraKeys[0]) {
		cameraNode->Translate(Urho3D::Vector3::FORWARD * timeStep * 5);
		changed = true;
	}
	if (cameraKeys[1]) {
		cameraNode->Translate(Urho3D::Vector3::BACK * timeStep);
		changed = true;
	}
	if (cameraKeys[2]) {
		cameraNode->Translate(Urho3D::Vector3::LEFT * timeStep * 3);
		changed = true;
	}
	if (cameraKeys[3]) {
		cameraNode->Translate(Urho3D::Vector3::RIGHT * timeStep * 3);
		changed = true;
	}
}

void FreeCameraBehave::rotate(const IntVector2& mouseMove, const double mouse_sensitivity) {
	yaw += mouse_sensitivity * mouseMove.x_;
	pitch += mouse_sensitivity * mouseMove.y_;
	//pitch_ = Clamp(pitch_, -90.0f, 90.0f);
	setRotation(Quaternion(pitch, yaw, 0.0f));
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
	return MM_RELATIVE;
}
