#pragma once
#include "FutureOrder.h"

class GroupOrder : public FutureOrder
{
public:
	GroupOrder(std::vector<Physical*>* entities, const Urho3D::Vector2& vector, const Physical* physical, UnitOrder action);
	~GroupOrder();
private:
	std::vector<Physical*>* entities;
	void addTargetAim(Urho3D::Vector2* to) override;
	void addFollowAim(const Physical* toFollow) override;
	void addChargeAim(Urho3D::Vector2* charge) override;
	void addDeadAim() override;

	void addAttackAim(const Physical* physical) override;
	void addDefendAim() override;
};
