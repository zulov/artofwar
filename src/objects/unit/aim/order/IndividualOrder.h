#pragma once
#include "objects/unit/aim/order/FutureOrder.h"

class IndividualOrder : public FutureOrder
{
public:
	IndividualOrder(Unit* unit, const Urho3D::Vector2& vector, const Physical* physical, UnitOrder action);
	~IndividualOrder();
private:
	Unit* unit;
};
