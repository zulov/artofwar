#pragma once
#include <Urho3D/Math/Vector3.h>

#include "Game.h"
#include "math/MathUtils.h"

struct MouseHeld {
	MouseHeld() = default;
	MouseHeld(const MouseHeld&) = delete;

	Urho3D::Vector3 first;
	Urho3D::Vector3 second;

	float sq2DDist() const {
		return sqDistAs2D(first, second);
	}

	std::pair<float, float> minMaxX() const {
		return std::minmax(first.x_, second.x_);
	}

	std::pair<float, float> minMaxZ() const {
		return std::minmax(first.z_, second.z_);
	}
};


struct MouseButton {
	MouseButton() = default;
	MouseButton(const MouseButton&) = delete;

	~MouseButton() { clean(); }

	void clean() {
		// if (held.first) {
		// 	delete held.first;
		// 	held.first = nullptr;
		// }
		// if (held.second) {
		// 	delete held.second;
		// 	held.second = nullptr;
		// }
		isHeld = false;
	}

	void setFirst(Urho3D::Vector3& hitPos) {
		// if (held.first != nullptr) {
		// 	delete held.first;
		// 	held.first = nullptr;
		// }
		held.first = hitPos;
		lastDown = Game::getTime();
		isHeld = true;
	}

	void setSecond(Urho3D::Vector3& hitPos) {
		// if (held.second != nullptr) {
		// 	delete held.second;
		// 	held.second = nullptr;
		// }
		held.second = hitPos;
		isHeld = false;
		lastUp = Game::getTime();
	}
	
	void markHeld() {
		isHeld = true;
	}

	bool isHeld = false;
	MouseHeld held;
	float lastUp = -1.f;
	float lastDown = -1.f;
};
