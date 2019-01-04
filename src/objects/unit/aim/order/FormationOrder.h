#pragma once
#include "FutureOrder.h"

class Formation;

class FormationOrder: public FutureOrder
{
public:
	FormationOrder(Formation * formation, const Urho3D::Vector2& vector, const Physical* physical, UnitOrder action);
	~FormationOrder();
	private:
	Formation* formation;
};

