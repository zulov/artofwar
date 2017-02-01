#include "FreeCameraBehave.h"



FreeCameraBehave::FreeCameraBehave(Urho3D::Context* context) {
	cameraNode = new Urho3D::Node(context);
	cameraNode->SetPosition(Urho3D::Vector3(0.0f, 10.0f, -10.0f));
	Urho3D::Camera* camera = cameraNode->CreateComponent<Urho3D::Camera>();
	camera->SetFarClip(300.0f);
}


FreeCameraBehave::~FreeCameraBehave() {}

void FreeCameraBehave::translate(Urho3D::Vector3 vector) {
	cameraNode->Translate(vector);
}

void FreeCameraBehave::rotate() {

}

void FreeCameraBehave::setRotate(const Urho3D::Quaternion& rotation) {
	cameraNode->SetRotation(rotation);
}
