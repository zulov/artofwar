#include "RtsCameraBehave.h"



RtsCameraBehave::RtsCameraBehave(Urho3D::Context* context) {
	cameraNode = new Urho3D::Node(context);
	cameraNode->SetPosition(Urho3D::Vector3(0.0f, 10.0f, -10.0f));
	cameraNode->SetDirection(Urho3D::Vector3::DOWN+ Urho3D::Vector3::FORWARD);
	Urho3D::Camera* camera = cameraNode->CreateComponent<Urho3D::Camera>();
	camera->SetFarClip(300.0f);
}


RtsCameraBehave::~RtsCameraBehave() {}

void RtsCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep) {
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
	if (wheel != 0) {
		Urho3D::Vector3 pos = cameraNode->GetPosition();
		double diff = pos.y_ - minY;
		cameraNode->Translate(Urho3D::Vector3::UP * timeStep*wheel*diff);
		pos = cameraNode->GetPosition();
		if (pos.y_ < minY) {
			pos.y_ = minY;
			
		} else if (pos.y_>maxY) {
			pos.y_ = maxY;
		}
		cameraNode->SetPosition(pos);
	}
	
}

void RtsCameraBehave::rotate() {

}

void RtsCameraBehave::setRotate(const Urho3D::Quaternion& rotation) {
	
}