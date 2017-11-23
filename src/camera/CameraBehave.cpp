#include "CameraBehave.h"
#include "Game.h"
#include "simulation/env/Enviroment.h"


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

const Vector3& CameraBehave::getPosition() {
	return cameraNode->GetPosition();
}

void CameraBehave::changePosition(float percentX, float percentY) {
	Vector3 pos = cameraNode->GetPosition();

	Vector3 newPos = Game::get()->getEnviroment()->getValidPosForCamera(percentX, percentY, pos, minY);

	cameraNode->SetPosition(newPos);
	changed = true;
}
