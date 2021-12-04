#include "CameraManager.h"
#include "FreeCameraBehave.h"
#include "Game.h"
#include "RtsCameraBehave.h"
#include "TopCameraBehave.h"
#include "CameraEnums.h"
#include "simulation/env/Environment.h"
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>

#include "simulation/SimGlobals.h"
#include "utils/OtherUtils.h"


CameraManager::CameraManager() {
	if (!SIM_GLOBALS.HEADLESS) {
		cameraBehaves[0] = new FreeCameraBehave();
		cameraBehaves[1] = new RtsCameraBehave();
		cameraBehaves[2] = new TopCameraBehave();

		activeBehave = cameraBehaves[1];


		float border = 256.f;
		auto graphics = Game::getGraphics();
		if (graphics) {
			const int width = Game::getGraphics()->GetWidth();
			const int height = Game::getGraphics()->GetHeight();
			widthEdge = width / border;
			heightEdge = height / border;

			widthEdgeMax = width - widthEdge;
			heightEdgeMax = height - heightEdge;
		}
	}
	camInfo = new CameraInfo();
}

CameraManager::~CameraManager() {
	clear_array(cameraBehaves, 3);
	delete camInfo;
}

void CameraManager::setCameraBehave(CameraBehaviorType _type) {
	auto& pos = activeBehave->getPosition();
	activeBehave = cameraBehaves[cast(_type)];
	activeBehave->setPos2D(pos);
	hasMoved = true;
}

Urho3D::Camera* CameraManager::getComponent() const {
	return activeBehave->getComponent();
}

void CameraManager::createCameraKeys(Urho3D::Input* input, bool cameraKeys[4],
                                     const Urho3D::IntVector2& cursorPos) const {
	cameraKeys[0] = input->GetKeyDown(Urho3D::KEY_W);
	cameraKeys[1] = input->GetKeyDown(Urho3D::KEY_S);
	cameraKeys[2] = input->GetKeyDown(Urho3D::KEY_A);
	cameraKeys[3] = input->GetKeyDown(Urho3D::KEY_D);

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

void CameraManager::translate(const Urho3D::IntVector2& cursorPos, Urho3D::Input* input, float timeStep) {
	bool cameraKeys[4];
	createCameraKeys(input, cameraKeys, cursorPos);
	const int wheel = input->GetMouseMoveWheel();
	const Urho3D::Vector3 pos = activeBehave->getPosition();
	const float min = Game::getEnvironment()->getGroundHeightAt(pos.x_, pos.z_);

	hasMoved = activeBehave->translate(cameraKeys, wheel, timeStep * MOVE_SPEED, min) || hasMoved;
	hasMoved = activeBehave->rotate(input->GetMouseMove(), MOUSE_SENSITIVITY) || hasMoved;
	if (hasMoved) {
		camInfo->info = activeBehave->getInfo();
	}
}

const Urho3D::String& CameraManager::getPosInfo() const {
	return camInfo->info;
}

Urho3D::MouseMode CameraManager::getMouseMode() const {
	return activeBehave->getMouseMode();
}

void CameraManager::changePositionInPercent(float x, float y) {
	activeBehave->changePositionInPercent(x, y);
	hasMoved = true;
	camInfo->info = activeBehave->getInfo();
}


void CameraManager::changePosition(float x, float y) {
	activeBehave->changePosition(x, y);
	hasMoved = true;
	camInfo->info = activeBehave->getInfo();
}

const Urho3D::Vector2 CameraManager::getTargetPos() const {
	return activeBehave->getTargetPos();
}

const CameraInfo* CameraManager::getCamInfo(float radius) {
	if (!SIM_GLOBALS.HEADLESS) {
		auto camPos = activeBehave->getTargetPos();
		camInfo->boundary = Urho3D::Vector4(camPos.x_ - radius, camPos.x_ + radius, camPos.y_ - radius,
		                                    camPos.y_ + radius);
		camInfo->hasMoved = hasMoved;
		hasMoved = false;
	} else {
		camInfo->boundary = Urho3D::Vector4::ZERO;
		camInfo->hasMoved = false;
		hasMoved = false;
	}

	return camInfo;
}
