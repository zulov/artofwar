#pragma once
#include "objects/unit/aim/order/FutureOrder.h"

class IndividualOrder: public FutureOrder
{
public:
	IndividualOrder(Unit* unit);
	~IndividualOrder();
};

