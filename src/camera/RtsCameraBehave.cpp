#include "RtsCameraBehave.h"
#include "Game.h"

RtsCameraBehave::RtsCameraBehave():CameraBehave(3,"RTSCam") {
	const double diff = sqrt(50.0f - minY) + 1;
	const double a = 10;
	cameraNode->SetDirection(Urho3D::Vector3::DOWN * diff + Urho3D::Vector3::FORWARD * a);
}


RtsCameraBehave::~RtsCameraBehave() = default;

void RtsCameraBehave::translate(bool cameraKeys[], int wheel, float timeStep, float min) {
	Urho3D::Vector3 pos = cameraNode->GetWorldPosition();
	float localMin = minY + min;
	float localMax = maxY + min;

	double diff;
	if (pos.y_ < localMin) {
		diff = 1;
	} else {
		diff = sqrt((pos.y_ - localMin) / 10) + 1;
	}

	if (cameraKeys[0]) {
		cameraNode->Translate(diff * Urho3D::Vector3::FORWARD * timeStep, Urho3D::TS_WORLD);
		changed = true;
	}
	if (cameraKeys[1]) {
		cameraNode->Translate(diff * Urho3D::Vector3::BACK * timeStep, Urho3D::TS_WORLD);
		changed = true;
	}
	if (cameraKeys[2]) {
		cameraNode->Translate(diff * Urho3D::Vector3::LEFT * timeStep, Urho3D::TS_WORLD);
		changed = true;
	}
	if (cameraKeys[3]) {
		cameraNode->Translate(diff * Urho3D::Vector3::RIGHT * timeStep, Urho3D::TS_WORLD);
		changed = true;
	}
	if (wheel != 0 || pos.y_ < localMin) {
		Urho3D::Vector3 pos2 = cameraNode->GetWorldPosition();
		if (pos2.y_ < localMin) {
			pos2.y_ = localMin;
		}
		double diff = sqrt(pos2.y_ - localMin) + 1;

		pos2 += Vector3::DOWN * wheel * diff * 1.5;

		if (pos2.y_ < localMin) {
			pos2.y_ = localMin;
		} else if (pos2.y_ > localMax) {
			pos2.y_ = localMax;
		}
		double a = 10;

		cameraNode->SetWorldPosition(pos2);
		cameraNode->SetDirection(Urho3D::Vector3::DOWN * diff + Urho3D::Vector3::FORWARD * a);
		changed = true;
	}

}

void RtsCameraBehave::rotate(const IntVector2& mouseMove, const double mouse_sensitivity) {
}

void RtsCameraBehave::setRotation(const Urho3D::Quaternion& rotation) {
}

Urho3D::String* RtsCameraBehave::getInfo() {
	if (changed) {
		(*info) = name + " \t" + cameraNode->GetPosition().ToString() + "\n" + cameraNode->GetRotation().ToString();
		changed = false;
	}
	return info;
}

Urho3D::MouseMode RtsCameraBehave::getMouseMode() {
	return MM_RELATIVE;
}
