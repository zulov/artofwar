#pragma once
#include <Urho3D/Math/Vector3.h>
#include <utility>
#include "Game.h"

struct MouseButton {
	MouseButton() = default;
	MouseButton(const MouseButton&) = delete;

	~MouseButton() { clean(); }

	void clean() {
		if (held.first) {
			delete held.first;
			held.first = nullptr;
		}
		if (held.second) {
			delete held.second;
			held.second = nullptr;
		}
		isHeld = false;
	}

	void setFirst(Urho3D::Vector3& hitPos) {
		if (held.first != nullptr) {
			delete held.first;
			held.first = nullptr;
		}
		held.first = new Urho3D::Vector3(hitPos);
		lastDown = Game::getTime();
		isHeld = true;
	}

	void setSecond(Urho3D::Vector3& hitPos) {
		if (held.second != nullptr) {
			delete held.second;
			held.second = nullptr;
		}
		held.second = new Urho3D::Vector3(hitPos);
		isHeld = false;
		lastUp = Game::getTime();
	}

	void markHeld() {
		isHeld = true;
	}

	void markIfNotHeld() {
		if (!isHeld) {
			isHeld = true;
		}
	}
	bool isHeld = false;
	std::pair<Urho3D::Vector3*, Urho3D::Vector3*> held;
	float lastUp = -1;
	float lastDown = -1;
};