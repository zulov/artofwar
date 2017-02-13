#include "FreeCameraBehave.h"



FreeCameraBehave::FreeCameraBehave(Urho3D::Context* context) {
	cameraNode = new Urho3D::Node(context);
	cameraNode->SetPosition(Urho3D::Vector3(0.0f, 10.0f, -10.0f));
	Urho3D::Camera* camera = cameraNode->CreateComponent<Urho3D::Camera>();
	camera->SetFarClip(300.0f);
	name = Urho3D::String("FreeCam");
}


FreeCameraBehave::~FreeCameraBehave() {}

void FreeCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep) {
	if (cameraKeys[0]) {
		cameraNode->Translate(Urho3D::Vector3::FORWARD * timeStep);
	}
	if (cameraKeys[1]) {
		cameraNode->Translate(Urho3D::Vector3::BACK * timeStep);
	}
	if (cameraKeys[2]) {
		cameraNode->Translate(Urho3D::Vector3::LEFT * timeStep);
	}
	if (cameraKeys[3]) {
		cameraNode->Translate(Urho3D::Vector3::RIGHT * timeStep);
	}
}

void FreeCameraBehave::rotate() {

}

void FreeCameraBehave::setRotate(const Urho3D::Quaternion& rotation) {
	cameraNode->SetRotation(rotation);
}

Urho3D::String FreeCameraBehave::getInfo() {
	return name +" \t" + cameraNode->GetPosition().ToString();
}

Urho3D::MouseMode FreeCameraBehave::getMouseMode() {
	return MM_RELATIVE;
}
