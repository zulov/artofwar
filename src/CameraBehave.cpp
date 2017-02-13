#include "CameraBehave.h"



CameraBehave::CameraBehave() {}


CameraBehave::~CameraBehave() {}

Urho3D::Camera * CameraBehave::getComponent() {
	return cameraNode->GetComponent<Urho3D::Camera>();
}
