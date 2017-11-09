#include "TopCameraBehave.h"
#include "Game.h"


TopCameraBehave::TopCameraBehave() {
	cameraNode = new Urho3D::Node(Game::get()->getContext());
	cameraNode->SetPosition(Urho3D::Vector3(0.0f, 50.0f, -50.0f));
	double diff = sqrt(50.0f - minY) + 1;

	cameraNode->SetDirection(Urho3D::Vector3::DOWN * diff);
	camera = cameraNode->CreateComponent<Urho3D::Camera>();
	camera->SetOrthographic(true);

	camera->SetFarClip(300.0f);
	name = Urho3D::String("TopCam");
}


TopCameraBehave::~TopCameraBehave() {
}

void TopCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep, float min) {
	double diff = sqrt((orthoSize - minY) / 10) + 1;

	if (cameraKeys[0]) {
		cameraNode->Translate(diff * Urho3D::Vector3::FORWARD * timeStep, Urho3D::TS_WORLD);
		changed = true;
	}
	if (cameraKeys[1]) {
		cameraNode->Translate(diff * Urho3D::Vector3::BACK * timeStep, Urho3D::TS_WORLD);
		changed = true;
	}
	if (cameraKeys[2]) {
		cameraNode->Translate(diff * Urho3D::Vector3::LEFT * timeStep, Urho3D::TS_WORLD);
		changed = true;
	}
	if (cameraKeys[3]) {
		cameraNode->Translate(diff * Urho3D::Vector3::RIGHT * timeStep, Urho3D::TS_WORLD);
		changed = true;
	}
	if (wheel != 0) {
		orthoSize -= wheel * (sqrt(orthoSize - minY) + 1) * 1.5;

		if (orthoSize < minY) {
			orthoSize = minY;
		} else if (orthoSize > maxY) {
			orthoSize = maxY;
		}
		camera->SetOrthoSize(orthoSize);
		
		changed = true;
	}
}

void TopCameraBehave::rotate(const IntVector2& mouseMove, const double mouse_sensitivity) {

}

void TopCameraBehave::setRotation(const Urho3D::Quaternion& rotation) {

}

Urho3D::String* TopCameraBehave::getInfo() {
	if (changed) {
		(*info) = name + " \t" + cameraNode->GetPosition().ToString() + "\n" + cameraNode->GetRotation().ToString();
		changed = false;
	}
	return info;
}

MouseMode TopCameraBehave::getMouseMode() {
	return MM_RELATIVE;
}
