#include "RtsCameraBehave.h"
#include "Game.h"


RtsCameraBehave::RtsCameraBehave() {
	cameraNode = new Urho3D::Node(Game::getInstance()->getContext());
	cameraNode->SetPosition(Urho3D::Vector3(0.0f, 10.0f, -10.0f));
	cameraNode->SetDirection(Urho3D::Vector3::DOWN + Urho3D::Vector3::FORWARD);
	Urho3D::Camera* camera = cameraNode->CreateComponent<Urho3D::Camera>();
	camera->SetFarClip(300.0f);
	name = Urho3D::String("RTSCam");
}


RtsCameraBehave::~RtsCameraBehave() {
}

void RtsCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep) {
	if (cameraKeys[0]) {
		cameraNode->Translate(Urho3D::Vector3::FORWARD * timeStep, Urho3D::TS_WORLD);
	}
	if (cameraKeys[1]) {
		cameraNode->Translate(Urho3D::Vector3::BACK * timeStep, Urho3D::TS_WORLD);
	}
	if (cameraKeys[2]) {
		cameraNode->Translate(Urho3D::Vector3::LEFT * timeStep, Urho3D::TS_WORLD);
	}
	if (cameraKeys[3]) {
		cameraNode->Translate(Urho3D::Vector3::RIGHT * timeStep, Urho3D::TS_WORLD);
	}
	if (wheel != 0) {
		Urho3D::Vector3 pos = cameraNode->GetWorldPosition();
		double diff = pos.y_ - minY;
		diff += 10;
		pos += Urho3D::Vector3(0, 1, 0) * timeStep * wheel * diff;

		if (pos.y_ < minY) {
			pos.y_ = minY;
		} else if (pos.y_ > maxY) {
			pos.y_ = maxY;
		}
		cameraNode->SetWorldPosition(pos);
		cameraNode->SetDirection((Urho3D::Vector3::DOWN + Urho3D::Vector3::FORWARD / diff * 20));
	}

}

void RtsCameraBehave::rotate(const IntVector2& mouseMove, const double mouse_sensitivity) {

}

void RtsCameraBehave::setRotation(const Urho3D::Quaternion& rotation) {

}

Urho3D::String RtsCameraBehave::getInfo() {
	return name + " \t" + cameraNode->GetPosition().ToString() + "\n" + cameraNode->GetRotation().ToString();
}

Urho3D::MouseMode RtsCameraBehave::getMouseMode() {
	return MM_FREE;
}
