#pragma once
#include <Urho3D/Math/Vector3.h>
#include <utility>

struct MouseButton
{
	MouseButton() {
		held = new std::pair<Vector3*, Vector3*>();
	}

	~MouseButton() {
		clean();
		delete held;
	}

	void clean() {
		if (held->first != nullptr) {
			delete held->first;
			held->first = nullptr;
		}
		if (held->second != nullptr) {
			delete held->second;
			held->second = nullptr;
		}
		isHeld = false;
	}

	void setFirst(Vector3& hitPos) {
		if (held->first != nullptr) {
			delete held->first;
			held->first = nullptr;
		}
		held->first = new Vector3(hitPos);
		isHeld = true;
	}

	void setSecond(Vector3& hitPos) {
		held->second = new Vector3(hitPos);
		isHeld = false;
	}

	void markHeld() {
		isHeld = true;
	}


	bool isHeld = false;

	std::pair<Urho3D::Vector3*, Urho3D::Vector3*>* held;
};
