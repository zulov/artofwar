#pragma once
#include "FutureOrder.h"

class GroupOrder : public FutureOrder
{
public:
	GroupOrder(std::vector<Physical*>* entities, const Urho3D::Vector2& vector, const Physical* physical, UnitOrder action);
	~GroupOrder();
	void add(bool append) override;
private:
	std::vector<Physical*>* entities;

	void addTargetAim() override;
	void addFollowAim() override;
	void addChargeAim() override;
	void addAttackAim() override;
	void addDefendAim() override;
	void addDeadAim() override;
};
