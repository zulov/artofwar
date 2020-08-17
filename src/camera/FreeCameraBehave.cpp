#include "FreeCameraBehave.h"
#include "Game.h"


FreeCameraBehave::FreeCameraBehave(): CameraBehave(Urho3D::Vector3(0.0f, 50.0f, -50.0f), 3, "FreeCam") {
	coefs[0] = 4;
	coefs[1] = 1;
	coefs[2] = 3;
	coefs[3] = 3;
}

void FreeCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep, float min) {
	translateInternal(cameraKeys, timeStep, 1);
}

void FreeCameraBehave::rotate(const Urho3D::IntVector2& mouseMove, const float mouse_sensitivity) {
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
