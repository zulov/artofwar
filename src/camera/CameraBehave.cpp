#include "CameraBehave.h"
#include "Game.h"
#include "simulation/env/Enviroment.h"


CameraBehave::CameraBehave(const Vector3& _pos, float _minY, const String& _name) {
	minY = _minY;
	name = _name;
	info = new String();
	changed = true;
	cameraNode = new Urho3D::Node(Game::getContext());
	cameraNode->SetPosition(_pos);
	camera = cameraNode->CreateComponent<Urho3D::Camera>();
	camera->SetFarClip(300.0f);
}

CameraBehave::~CameraBehave() {
	delete info;
	cameraNode->Remove();
}

Urho3D::Camera* CameraBehave::getComponent() {
	return cameraNode->GetComponent<Urho3D::Camera>();
}

const Vector3& CameraBehave::getPosition() {
	return cameraNode->GetPosition();
}

void CameraBehave::changePosition(float percentX, float percentY) {
	const Vector3 pos = cameraNode->GetPosition();

	const Vector3 newPos = Game::getEnviroment()->getValidPosForCamera(percentX, percentY, pos, minY);

	cameraNode->SetPosition(newPos);
	changed = true;
}
