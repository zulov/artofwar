#include "RtsCameraBehave.h"



RtsCameraBehave::RtsCameraBehave(Urho3D::Context* context) {
	cameraNode = new Urho3D::Node(context);
	cameraNode->SetPosition(Urho3D::Vector3(0.0f, 10.0f, -10.0f));
	cameraNode->SetDirection(Urho3D::Vector3::DOWN+ Urho3D::Vector3::FORWARD);
	Urho3D::Camera* camera = cameraNode->CreateComponent<Urho3D::Camera>();
	camera->SetFarClip(300.0f);
}


RtsCameraBehave::~RtsCameraBehave() {}

void RtsCameraBehave::translate(Urho3D::Vector3 vector) {
	cameraNode->Translate(vector);
}

void RtsCameraBehave::rotate() {

}

void RtsCameraBehave::setRotate(const Urho3D::Quaternion& rotation) {
	
}