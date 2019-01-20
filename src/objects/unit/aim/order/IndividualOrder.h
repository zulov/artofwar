#pragma once
#include "objects/unit/aim/order/FutureOrder.h"

class IndividualOrder : public FutureOrder
{
public:
	IndividualOrder(Unit* unit, const Urho3D::Vector2& vector, const Physical* toUse, UnitOrder action);
	~IndividualOrder();
private:
	void addTargetAim(Urho3D::Vector2 to) override;
	void addFollowAim(const Physical* toFollow) override;
	void addChargeAim(Urho3D::Vector2 charge) override;
	void addDeadAim() override;
	void addAttackAim(const Physical* physical) override;
	void addDefendAim() override;

	Unit* unit;
};
