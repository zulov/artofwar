#pragma once
#include <Urho3D/Math/Vector3.h>
#include <utility>

struct MouseButton
{
	MouseButton() = default;

	~MouseButton() {
		clean();
	}

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
		isHeld = true;
	}

	void setSecond(Urho3D::Vector3& hitPos) {
		if (held.second != nullptr) {
			delete held.second;
			held.second = nullptr;
		}
		held.second = new Urho3D::Vector3(hitPos);
		isHeld = false;
	}

	void markHeld() {
		isHeld = true;
	}


	bool isHeld = false;
	std::pair<Urho3D::Vector3*, Urho3D::Vector3*> held;
};
