#include "CameraManager.h"

CameraManager::CameraManager() {	
	CameraBehave * cameraBehave = new FreeCameraBehave();
	cameraBehaves.push_back(cameraBehave);

	cameraBehave = new RtsCameraBehave();
	cameraBehaves.push_back(cameraBehave);

	activeBehave = cameraBehaves.at(1);
}

CameraManager::~CameraManager() {
	while (!cameraBehaves.empty()) {
		delete cameraBehaves.back();
		cameraBehaves.pop_back();
	}
}


void CameraManager::setCameraBehave(int _type) {
	activeBehave = cameraBehaves.at(_type);
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

void CameraManager::rotate(const IntVector2& mouse_move) {
	activeBehave->rotate(mouse_move, MOUSE_SENSITIVITY);
}
