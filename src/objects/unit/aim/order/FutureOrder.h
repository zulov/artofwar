#pragma once
#include "UnitOrder.h"
#include "objects/Physical.h"

class FutureOrder
{
	FutureOrder(const Urho3D::Vector2& vector, const Physical* physical, UnitOrder action);
	FutureOrder(const Urho3D::Vector2& vector, UnitOrder action);
	FutureOrder(const Physical* physical, UnitOrder action);
	bool expired() const;
	void execute(FutureOrder& futureAim);

	Urho3D::Vector2 vector;
	const Physical* physical;
	UnitOrder action;
};
