#pragma once
#include "FutureOrder.h"

class Formation;

class FormationOrder : public FutureOrder
{
public:
	FormationOrder(Formation* formation, const Urho3D::Vector2& vector, const Physical* physical, UnitOrder action);
	~FormationOrder();
private:
	void addTargetAim(Urho3D::Vector2 to) override;
	void addFollowAim(const Physical* toFollow) override;
	void addChargeAim(Urho3D::Vector2 charge) override;
	void addDeadAim() override;
	void addAttackAim(const Physical* physical) override;
	void addDefendAim() override;
	Formation* formation;
};
