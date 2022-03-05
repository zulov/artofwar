#include "CameraBehave.h"

#include "Game.h"
#include "math/MathUtils.h"
#include "env/Environment.h"
#include "utils/consts.h"


CameraBehave::CameraBehave(const Urho3D::Vector3& pos, float minY, Urho3D::String name):
	name(std::move(name)), minY(minY) {

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
	cameraNode->Remove();
}

Urho3D::Camera* CameraBehave::getComponent() const {
	return cameraNode->GetComponent<Urho3D::Camera>();
}

Urho3D::String CameraBehave::getInfo() const {
	return name + " \t" + cameraNode->GetPosition().ToString() + "\n" + cameraNode->GetRotation().ToString();
}

const Urho3D::Vector3& CameraBehave::getPosition() const {
	return cameraNode->GetPosition();
}

void CameraBehave::changeTargetInPercent(float percentX, float percentY) const {
	const auto pos = Game::getEnvironment()->getPosFromPercent(percentX, percentY);
	changeTarget(pos.x_, pos.y_);
}

void CameraBehave::changeTarget(float x, float z) const {
	const auto currentTarget = getTargetPos();
	const auto diff = Urho3D::Vector2(x, z) - currentTarget;

	const auto currentPos = cameraNode->GetPosition();

	const auto newPos = Urho3D::Vector2(currentPos.x_ + diff.x_, currentPos.z_ + diff.y_);
	
	float h = Game::getEnvironment()->getGroundHeightAt(newPos.x_, newPos.y_);
	auto y = Urho3D::Max(h + minY, cameraNode->GetPosition().y_);

	setPos(to3D(newPos, y));
}

void CameraBehave::setPos(const Urho3D::Vector3& newPos) const {
	cameraNode->SetPosition(newPos);
}

Urho3D::Vector2 CameraBehave::getTargetPos() const {
	return to2D(cameraNode->GetPosition());
}

void CameraBehave::translateCam(float timeStep, float diff, Urho3D::Vector3 dir) const {
	cameraNode->Translate(diff * dir * timeStep, Urho3D::TS_WORLD);
}

bool CameraBehave::translateInternal(const bool* cameraKeys, float timeStep, float diff) {
	bool hasMoved = false;
	for (int i = 0; i < 4; ++i) {
		if (cameraKeys[i]) {
			translateCam(timeStep, coefs[i] * diff, Consts::DIRS[i]);
			hasMoved = true;
		}
	}
	return hasMoved;
}
