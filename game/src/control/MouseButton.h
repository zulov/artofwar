#pragma once
#include <Urho3D/Math/Vector3.h>

#include "Game.h"
#include "math/MathUtils.h"

struct MouseHeld {
	MouseHeld() = default;
	MouseHeld(const MouseHeld&) = delete;

	Urho3D::Vector3 first;
	Urho3D::Vector3 second;

	Urho3D::Vector2 firstAs2D() const {
		return {first.x_, first.z_};
	}

	Urho3D::Vector2 secondAs2D() const {
		return {second.x_, second.z_};
	}

	float sq2DDist() const {
		return sqDistAs2D(first, second);
	}

	Urho3D::Vector2 first2Second() const {
		return {second.x_ - first.x_, second.z_ - first.z_};
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

	void setFirst(Urho3D::Vector3& hitPos) {
		held.first = hitPos;
		isHeld = true;
		prevDown = lastDown;
		lastDown = Game::getTime();
	}

	void setSecond(Urho3D::Vector3& hitPos) {
		held.second = hitPos;
		isHeld = false;
		prevUp = lastUp;
		lastUp = Game::getTime();
	}

	void markHeld() {
		isHeld = true;
	}

	float timeUpDiff() const {
		return lastUp - prevUp;
	}

	float sq2DDist() const {
		return held.sq2DDist();
	}

	MouseHeld held;
	float lastUp = -1.f;
	float prevUp = -1.f;
	float lastDown = -1.f;
	float prevDown = -1.f;
	bool isHeld = false;
};
