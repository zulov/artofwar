#include "CameraBehave.h"
#include "Game.h"
#include "simulation/env/Enviroment.h"


CameraBehave::CameraBehave(float _minY, String _name) {
	minY = _minY;
	name = _name;
	info = new String();
	changed = true;
	cameraNode = new Urho3D::Node(Game::get()->getContext());
	cameraNode->SetPosition(Urho3D::Vector3(0.0f, 50.0f, -50.0f));
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
	Vector3 pos = cameraNode->GetPosition();

	Vector3 newPos = Game::get()->getEnviroment()->getValidPosForCamera(percentX, percentY, pos, minY);

	cameraNode->SetPosition(newPos);
	changed = true;
}
