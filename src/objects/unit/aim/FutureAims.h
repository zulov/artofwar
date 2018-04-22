#pragma once
#include "OrderType.h"
#include "objects/Physical.h"

class Physical;

struct FutureAims
{
	Urho3D::Vector2 vector;
	Physical* physical;
	OrderType action;


	FutureAims(Vector2& vector, Physical* physical, OrderType action)
		: vector(vector),
		physical(physical),
		action(action) {
	}

	bool expired() {
		if (physical != nullptr && !physical->isAlive()) {
			return true;
		}
		return false;
	}
};
