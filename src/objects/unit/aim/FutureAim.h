#pragma once
#include "OrderType.h"
#include "objects/Physical.h"

struct FutureAim
{
	Urho3D::Vector2 vector;
	Physical* physical;
	OrderType action;


	FutureAim(Vector2& vector, Physical* physical, OrderType action)
		: vector(vector),
		physical(physical),
		action(action) {
	}

	bool expired() const {
		return physical != nullptr && !physical->isAlive();
	}
};
