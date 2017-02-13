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



Urho3D::Camera *CameraManager::getComponent() {
	//return cameraNode->GetComponent<Camera>();
	return activeBehave->getComponent();
}

void CameraManager::translate(bool cameraKeys[], int wheel, float timeStep) {
	activeBehave->translate(cameraKeys, wheel, timeStep*MOVE_SPEED);
}

String CameraManager::getInfo() {
	return activeBehave->getInfo();
}

Urho3D::MouseMode CameraManager::getMouseMode() {
	return activeBehave->getMouseMode();
}
