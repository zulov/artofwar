#pragma once
#include "OrderType.h"
#include "objects/Physical.h"

struct FutureAim
{
	Vector2 vector;
	const Physical* physical;
	OrderType action;


	FutureAim(const Urho3D::Vector2& vector, const Physical* physical, OrderType action)
		: vector(vector),
		physical(physical),
		action(action) {
	}

	FutureAim(const Urho3D::Vector2& vector, OrderType action)
		: vector(vector),
		action(action) {
	}

	FutureAim(Physical* physical, OrderType action)
		: physical(physical),
		action(action) {
	}

	bool expired() const {
		return physical != nullptr && !physical->isAlive();
	}
};
