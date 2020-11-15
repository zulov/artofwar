#include "CameraBehave.h"

#include <utility>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Scene/Node.h>
#include "Game.h"
#include "simulation/env/Environment.h"


CameraBehave::CameraBehave(const Urho3D::Vector3& pos, float minY, Urho3D::String name):
	name(std::move(name)), changed(true), minY(minY) {

	info = new Urho3D::String();

	cameraNode = new Urho3D::Node(Game::getContext());
	cameraNode->SetPosition(pos);
	camera = cameraNode->CreateComponent<Urho3D::Camera>();
	camera->SetFarClip(300.0f);
	coefs[0] = 1;
	coefs[1] = 1;
	coefs[2] = 1;
	coefs[3] = 1;
}

CameraBehave::~CameraBehave() {
	delete info;
	cameraNode->Remove();
}

Urho3D::Camera* CameraBehave::getComponent() const {
	return cameraNode->GetComponent<Urho3D::Camera>();
}

const Urho3D::Vector3& CameraBehave::getPosition() const {
	return cameraNode->GetPosition();
}

void CameraBehave::changePosition(float percentX, float percentY) {
	const auto newPos = Game::getEnvironment()->
		getValidPosForCamera(percentX, percentY, cameraNode->GetPosition(), minY);

	setPos2D(newPos);
}

void CameraBehave::setPos2D(const Urho3D::Vector3& newPos) {
	cameraNode->SetPosition(newPos);
	changed = true;
}

Urho3D::Vector2 CameraBehave::getTargetPos() const {
	auto pos = cameraNode->GetPosition();
	return Urho3D::Vector2(pos.x_, pos.z_);
}

void CameraBehave::translateCam(float timeStep, float diff, Urho3D::Vector3 dir) {
	cameraNode->Translate(diff * dir * timeStep, Urho3D::TS_WORLD);
	changed = true;
}

void CameraBehave::translateInternal(const bool* cameraKeys, float timeStep, float diff) {
	for (int i = 0; i < 4; ++i) {
		if (cameraKeys[i]) {
			translateCam(timeStep, coefs[i] * diff, dirs[i]);
		}
	}
}
