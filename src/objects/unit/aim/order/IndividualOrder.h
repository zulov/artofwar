#pragma once
#include "objects/unit/aim/order/FutureOrder.h"

class IndividualOrder : public FutureOrder
{
public:
	IndividualOrder(Unit* unit, const Urho3D::Vector2& vector, const Physical* toUse, UnitOrder action);
	~IndividualOrder();
	bool add(bool append) override;
private:
	void addTargetAim() override;
	void addFollowAim() override;
	void addChargeAim() override;
	void addAttackAim() override;
	void addDefendAim() override;
	void addDeadAim() override;
	void simpleAction();

	Unit* unit;
};
