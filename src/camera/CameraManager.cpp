#include "CameraManager.h"
#include "Game.h"
#include <Urho3D/Graphics/Graphics.h>
#include "TopCameraBehave.h"
#include "simulation/env/Enviroment.h"

CameraManager::CameraManager() {

	cameraBehaves.push_back(new FreeCameraBehave());
	cameraBehaves.push_back(new RtsCameraBehave());
	cameraBehaves.push_back(new TopCameraBehave());

	activeBehave = cameraBehaves.at(1);
	float border = 256.f;

	int width = Game::get()->getGraphics()->GetWidth();
	int height = Game::get()->getGraphics()->GetHeight();
	widthEdge = width / border;
	heightEdge = height / border;

	widthEdgeMax = width - widthEdge;
	heightEdgeMax = height - heightEdge;
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

Urho3D::Camera* CameraManager::getComponent() {
	return activeBehave->getComponent();
}

void CameraManager::createCameraKeys(Input* input, bool cameraKeys[4], const IntVector2& cursorPos) {
	cameraKeys[0] = input->GetKeyDown(KEY_W);
	cameraKeys[1] = input->GetKeyDown(KEY_S);
	cameraKeys[2] = input->GetKeyDown(KEY_A);
	cameraKeys[3] = input->GetKeyDown(KEY_D);

	if (cursorPos.x_ < widthEdge) {
		cameraKeys[2] = true;
	} else if (cursorPos.x_ > widthEdgeMax) {
		cameraKeys[3] = true;
	}
	if (cursorPos.y_ < heightEdge) {
		cameraKeys[0] = true;
	} else if (cursorPos.y_ > heightEdgeMax) {
		cameraKeys[1] = true;
	}
}

void CameraManager::translate(const IntVector2& cursorPos, Input* input, float timeStep) {
	bool cameraKeys[4];
	createCameraKeys(input, cameraKeys, cursorPos);
	int wheel = input->GetMouseMoveWheel();
	Vector3 pos = activeBehave->getPosition();
	float min = Game::get()->getEnviroment()->getGroundHeightAt(pos.x_, pos.z_);
	activeBehave->translate(cameraKeys, wheel, timeStep * MOVE_SPEED, min);
}

String* CameraManager::getInfo() {
	return activeBehave->getInfo();
}

Urho3D::MouseMode CameraManager::getMouseMode() {
	return activeBehave->getMouseMode();
}

void CameraManager::rotate(const IntVector2& mouse_move) {
	activeBehave->rotate(mouse_move, MOUSE_SENSITIVITY);
}

void CameraManager::changePosition(float x, float y) {
	activeBehave->changePosition(x, y);
}
