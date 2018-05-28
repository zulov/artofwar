#pragma once
#include "UnitOrder.h"
#include "objects/Physical.h"

struct FutureAim
{
	Vector2 vector;
	const Physical* physical;
	UnitOrder action;


	FutureAim(const Urho3D::Vector2& vector, const Physical* physical, UnitOrder action)
		: vector(vector),
		physical(physical),
		action(action) {
	}

	FutureAim(const Urho3D::Vector2& vector, UnitOrder action)
		: vector(vector),
		action(action) {
	}

	FutureAim(Physical* physical, UnitOrder action)
		: physical(physical),
		action(action) {
	}

	bool expired() const {
		return physical != nullptr && !physical->isAlive();
	}
};
