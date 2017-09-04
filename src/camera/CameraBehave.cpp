#include "CameraBehave.h"


CameraBehave::CameraBehave() {
	info = new String();
	changed = true;
}


CameraBehave::~CameraBehave() {
	delete info;
}

Urho3D::Camera* CameraBehave::getComponent() {
	return cameraNode->GetComponent<Urho3D::Camera>();
}
