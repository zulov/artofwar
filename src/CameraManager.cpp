#include "CameraManager.h"

CameraManager::CameraManager(Urho3D::Context* context) {	
	CameraBehave * cameraBehave = new FreeCameraBehave(context);
	cameraBehaves.push_back(cameraBehave);

	cameraBehave = new RtsCameraBehave(context);
	cameraBehaves.push_back(cameraBehave);

	activeBehave = cameraBehaves.at(0);
}

CameraManager::~CameraManager() {}


void CameraManager::setCameraBehave(int _type) {
	activeBehave = cameraBehaves.at(_type);
}

void CameraManager::setRotation(const Urho3D::Quaternion& rotation) {
	activeBehave->setRotate(rotation);
}

void CameraManager::translate(Urho3D::Vector3 vector) {
	activeBehave->translate(vector);
}

Urho3D::Camera *CameraManager::getComponent() {
	//return cameraNode->GetComponent<Camera>();
	return activeBehave->getComponent();
}

